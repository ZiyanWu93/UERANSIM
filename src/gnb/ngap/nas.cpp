//
// This file is a part of UERANSIM project.
// Copyright (c) 2023 ALİ GÜNGÖR.
//
// https://github.com/aligungr/UERANSIM/
// See README, LICENSE, and CONTRIBUTING files for licensing details.
//

#include "encode.hpp"
#include "task.hpp"
#include "utils.hpp"

#include <gnb/rrc/task.hpp>

#include "encode.hpp"
#include <asn/ngap/ASN_NGAP_DownlinkNASTransport.h>
#include <asn/ngap/ASN_NGAP_InitialUEMessage.h>
#include <asn/ngap/ASN_NGAP_InitiatingMessage.h>
#include <asn/ngap/ASN_NGAP_NASNonDeliveryIndication.h>
#include <asn/ngap/ASN_NGAP_NGAP-PDU.h>
#include <asn/ngap/ASN_NGAP_ProtocolIE-Field.h>
#include <asn/ngap/ASN_NGAP_RerouteNASRequest.h>
#include <asn/ngap/ASN_NGAP_UplinkNASTransport.h>
#include <cstring>
#include <stdexcept>
#include <ue/nas/enc.hpp>
#include <unistd.h>

namespace nr::gnb
{

int32_t extractSliceInfoAndModifyPdu(OctetString &nasPdu)
{
    nas::RegistrationRequest *regRequest = nullptr;
    int32_t requestedSliceType = -1;
    const uint8_t *m_data = nasPdu.data();
    size_t m_dataLength = nasPdu.length();
    OctetView octetView(m_data, m_dataLength);
    auto nasMessage = nas::DecodeNasMessage(octetView);
    if (nasMessage->epd == nas::EExtendedProtocolDiscriminator::MOBILITY_MANAGEMENT_MESSAGES)
    {
        nas::MmMessage *mmMessage = dynamic_cast<nas::MmMessage *>(nasMessage.get());
        if (mmMessage)
        {
            nas::PlainMmMessage *plainMmMessage = dynamic_cast<nas::PlainMmMessage *>(mmMessage);
            if (plainMmMessage)
            {
                regRequest = dynamic_cast<nas::RegistrationRequest *>(plainMmMessage);
                if (regRequest)
                {
                    auto sz = regRequest->requestedNSSAI->sNssais.size();
                    if (sz > 0)
                    {
                        requestedSliceType = static_cast<uint8_t>(regRequest->requestedNSSAI->sNssais[0].sst);
                    }
                }
            }
        }
    }
    if (regRequest && regRequest->requestedNSSAI)
        regRequest->requestedNSSAI = std::nullopt;

    OctetString modifiedNasPdu;
    nas::EncodeNasMessage(*nasMessage, modifiedNasPdu);
    nasPdu = std::move(modifiedNasPdu);
    return requestedSliceType;
}

void NgapTask::handleInitialNasTransport(int ueId, OctetString &nasPdu, int64_t rrcEstablishmentCause,
                                         const std::optional<GutiMobileIdentity> &sTmsi)
{
    int32_t requestedSliceType = extractSliceInfoAndModifyPdu(nasPdu);

    m_logger->debug("Initial NAS message received from UE[%d]", ueId);

    if (m_ueCtx.count(ueId))
    {
        m_logger->err("UE context[%d] already exists", ueId);
        return;
    }

    createUeContext(ueId, requestedSliceType);

    auto *ueCtx = findUeContext(ueId);
    if (ueCtx == nullptr)
        return;
    auto *amfCtx = findAmfContext(ueCtx->associatedAmfId);
    if (amfCtx == nullptr)
        return;

    if (amfCtx->state != EAmfState::CONNECTED)
    {
        m_logger->err("Initial NAS transport failure. AMF is not in connected state.");
        return;
    }

    amfCtx->nextStream = (amfCtx->nextStream + 1) % amfCtx->association.outStreams;
    if ((amfCtx->nextStream == 0) && (amfCtx->association.outStreams > 1))
        amfCtx->nextStream += 1;
    ueCtx->uplinkStream = amfCtx->nextStream;

    std::vector<ASN_NGAP_InitialUEMessage_IEs *> ies;

    auto *ieEstablishmentCause = asn::New<ASN_NGAP_InitialUEMessage_IEs>();
    ieEstablishmentCause->id = ASN_NGAP_ProtocolIE_ID_id_RRCEstablishmentCause;
    ieEstablishmentCause->criticality = ASN_NGAP_Criticality_ignore;
    ieEstablishmentCause->value.present = ASN_NGAP_InitialUEMessage_IEs__value_PR_RRCEstablishmentCause;
    ieEstablishmentCause->value.choice.RRCEstablishmentCause = rrcEstablishmentCause;
    ies.push_back(ieEstablishmentCause);

    auto *ieCtxRequest = asn::New<ASN_NGAP_InitialUEMessage_IEs>();
    ieCtxRequest->id = ASN_NGAP_ProtocolIE_ID_id_UEContextRequest;
    ieCtxRequest->criticality = ASN_NGAP_Criticality_ignore;
    ieCtxRequest->value.present = ASN_NGAP_InitialUEMessage_IEs__value_PR_UEContextRequest;
    ieCtxRequest->value.choice.UEContextRequest = ASN_NGAP_UEContextRequest_requested;
    ies.push_back(ieCtxRequest);

    auto *ieNasPdu = asn::New<ASN_NGAP_InitialUEMessage_IEs>();
    ieNasPdu->id = ASN_NGAP_ProtocolIE_ID_id_NAS_PDU;
    ieNasPdu->criticality = ASN_NGAP_Criticality_reject;
    ieNasPdu->value.present = ASN_NGAP_InitialUEMessage_IEs__value_PR_NAS_PDU;
    asn::SetOctetString(ieNasPdu->value.choice.NAS_PDU, nasPdu);
    ies.push_back(ieNasPdu);

    if (sTmsi)
    {
        auto *ieTmsi = asn::New<ASN_NGAP_InitialUEMessage_IEs>();
        ieTmsi->id = ASN_NGAP_ProtocolIE_ID_id_FiveG_S_TMSI;
        ieTmsi->criticality = ASN_NGAP_Criticality_reject;
        ieTmsi->value.present = ASN_NGAP_InitialUEMessage_IEs__value_PR_FiveG_S_TMSI;

        asn::SetBitStringInt<10>(sTmsi->amfSetId, ieTmsi->value.choice.FiveG_S_TMSI.aMFSetID);
        asn::SetBitStringInt<6>(sTmsi->amfPointer, ieTmsi->value.choice.FiveG_S_TMSI.aMFPointer);
        asn::SetOctetString4(ieTmsi->value.choice.FiveG_S_TMSI.fiveG_TMSI, sTmsi->tmsi);
        ies.push_back(ieTmsi);
    }

    auto *pdu = asn::ngap::NewMessagePdu<ASN_NGAP_InitialUEMessage>(ies);
    // sendNgapUeAssociated(ueId, pdu);
    // sleep for 0.3 seconds.
    usleep(300000);
    deliverDownlinkNasRefactored();
}

// void NgapTask::deliverDownlinkNas(int ueId, OctetString &&nasPdu)
// {
//     auto w = std::make_unique<NmGnbNgapToRrc>(NmGnbNgapToRrc::NAS_DELIVERY);
//     w->ueId = ueId;
//     w->pdu = std::move(nasPdu);
//     m_base->rrcTask->push(std::move(w));
// }

int times = 1;
void NgapTask::deliverDownlinkNas(int ueId, OctetString &&nasPdu)
{
    return;
}

constexpr size_t MAX_NAS_HEX_LEN = 300;             // big enough for the longest hard-coded hex string
static char nas_payload_buf[MAX_NAS_HEX_LEN] = {0}; // single reusable buffer

static const char *prepare_payload(const char *src)
{
    std::strncpy(nas_payload_buf, src, MAX_NAS_HEX_LEN - 1);
    nas_payload_buf[MAX_NAS_HEX_LEN - 1] = '\0'; // ensure null-termination
    return nas_payload_buf;
}

const char *generate_auth_req()
{
    // Prepare parts using C-style character arrays (modifiable).
    char epd_header[] = "7e00"; // Extended Protocol Discriminator (126) + spare + security header type (plain)
    char msg_type[] = "56";     // Message Type = Authentication Request
    char ngksi_field[] = "00";  // NAS Key Set Identifier (ngKSI) + spare half-octet (native, id=0)
    char abba_ie[] = "020000";  // ABBA IE: length 2, value 0x0000
    char rand_iei[] = "21";     // IEI for RAND
    char rand_val[] = "5ca0df8c9bb8dbcf3c2a7dd448da1369"; // 128-bit RAND challenge (example)
    char autn_iei[] = "20";                               // IEI for AUTN
    char autn_len[] = "10";                               // AUTN length = 16 bytes (0x10)
    char autn_val[] = "406296993082800030b762455c890b19"; // AUTN value (SQN⊕AK|AMF|MAC)

    char *parts[] = {epd_header, msg_type, ngksi_field, abba_ie, rand_iei, rand_val, autn_iei, autn_len, autn_val};

    size_t offset = 0;
    for (size_t i = 0; i < sizeof(parts) / sizeof(parts[0]); ++i)
    {
        const char *part = parts[i];
        size_t len = std::strlen(part);
        if (offset + len >= MAX_NAS_HEX_LEN)
        {
            // Should never happen; guard to prevent overflow.
            nas_payload_buf[0] = '\0';
            return nas_payload_buf;
        }
        std::memcpy(nas_payload_buf + offset, part, len);
        offset += len;
    }
    nas_payload_buf[offset] = '\0'; // null-terminate
    return nas_payload_buf;
}

const char *generate_security_cmd()
{
    // Prepare parts using C-style character arrays (modifiable).
    char outer_header[] = "7e03"; // Extended Protocol Discriminator + Security header type (3)
    char mac[] = "13bf995a";      // Message authentication code (example)
    char seq_num[] = "00";        // Sequence number

    char inner_header[] = "7e00"; // Extended Protocol Discriminator + Security header type (0)
    char msg_type[] = "5d";       // Message Type = Security Mode Command

    char nas_alg[] = "02";     // NAS security algorithms: ciphering (0), integrity (2)
    char ngksi_field[] = "00"; // NAS Key Set Identifier (ngKSI) + spare half-octet

    char ue_sec_cap_len[] = "04";       // UE security capability length
    char ue_sec_cap_val[] = "80f080f0"; // UE security capabilities bitmap

    char imeisv_req[] = "e1";       // IMEISV request IE: ID (0xe), value (0x1)
    char add_sec_info[] = "360102"; // Additional 5G security information: IEI (0x36), length (1), value (0x02)

    char *parts[] = {outer_header,   mac,        seq_num,     inner_header,
                     msg_type,       nas_alg,    ngksi_field, ue_sec_cap_len,
                     ue_sec_cap_val, imeisv_req, add_sec_info};

    size_t offset = 0;
    for (size_t i = 0; i < sizeof(parts) / sizeof(parts[0]); ++i)
    {
        const char *part = parts[i];
        size_t len = std::strlen(part);
        if (offset + len >= MAX_NAS_HEX_LEN)
        {
            nas_payload_buf[0] = '\0';
            return nas_payload_buf;
        }
        std::memcpy(nas_payload_buf + offset, part, len);
        offset += len;
    }
    nas_payload_buf[offset] = '\0'; // null-terminate
    return nas_payload_buf;
}

const char *generate_registration_accept()
{
    /* Outer (security-protected) header */
    char outer_header[] = "7e02"; /* EPD 0x7e, security hdr type 0x02 */
    char mac[] = "7239674c";      /* 32-bit message authentication code */
    char seq_num[] = "01";        /* Sequence number */

    /* Inner plain NAS header */
    char inner_header[] = "7e00"; /* EPD 0x7e, plain message */
    char msg_type[] = "42";       /* Registration Accept */

    /* Information elements */
    char reg_result[] = "0101";                      /* IEI 0x01, value 0x01 */
    char guti_ie[] = "77000bf299f907020040c0000727"; /* 5G-GUTI, length 0x000b */
    char tai_list[] = "54074099f907000001";          /* TAI list, length 0x07 */
    char nssai[] = "15020101";                       /* Allowed NSSAI */
    char net_feat[] = "21020100";                    /* Network feature support */
    char gprs_timer[] = "5e0192";                    /* T3512 value: 540 s */

    /* Assemble */
    char *parts[] = {outer_header, mac,      seq_num, inner_header, msg_type,  reg_result,
                     guti_ie,      tai_list, nssai,   net_feat,     gprs_timer};

    size_t offset = 0;
    for (size_t i = 0; i < sizeof(parts) / sizeof(parts[0]); ++i)
    {
        size_t len = std::strlen(parts[i]);
        if (offset + len >= MAX_NAS_HEX_LEN)
        {
            nas_payload_buf[0] = '\0';
            return nas_payload_buf;
        }
        std::memcpy(nas_payload_buf + offset, parts[i], len);
        offset += len;
    }
    nas_payload_buf[offset] = '\0';
    return nas_payload_buf;
}

const char *generate_configuration_update()
{
    /* outer security-protected header */
    char outer_header[] = "7e02"; /* EPD 0x7e, integrity-ciphered (2)       */
    char mac[] = "de0d22e3";      /* message authentication code            */
    char seq_num[] = "02";        /* sequence number                        */

    /* inner plain NAS header */
    char inner_header[] = "7e00"; /* EPD 0x7e, plain message                */
    char msg_type[] = "54";       /* configuration update command           */

    /* information elements */
    char net_name_full[] = "430f90004f00700065006e003500470053"; /* “Open5GS” */
    char net_name_short[] = "450990004e006500780074";            /* “Next”    */
    char tz_local[] = "460a";                                    /* GMT-5     */
    char tz_and_time[] = "475260903035530a";                     /* 2025-06-09 03:53:35, GMT-5 */
    char dst[] = "490101";                                       /* DST +1 h  */

    char *parts[] = {outer_header,   mac,      seq_num,     inner_header, msg_type, net_name_full,
                     net_name_short, tz_local, tz_and_time, dst};

    size_t offset = 0;
    for (size_t i = 0; i < sizeof(parts) / sizeof(parts[0]); ++i)
    {
        size_t len = std::strlen(parts[i]);
        if (offset + len >= MAX_NAS_HEX_LEN)
        {
            nas_payload_buf[0] = '\0';
            return nas_payload_buf;
        }
        std::memcpy(nas_payload_buf + offset, parts[i], len);
        offset += len;
    }
    nas_payload_buf[offset] = '\0';
    return nas_payload_buf;
}

const char *generate_pdu_session_establishment()
{
    /* outer security-protected header */
    char outer_hdr[] = "7e02"; /* EPD 0x7e, integrity-protected & ciphered (2) */
    char mac[] = "fbd62d81";   /* message authentication code                  */
    char seq[] = "03";         /* sequence number                              */

    /* inner plain NAS header (DL NAS Transport) */
    char inner_hdr[] = "7e00"; /* EPD 0x7e, plain message                      */
    char dl_msg[] = "68";      /* DL NAS transport                             */

    /* DL transport payload-container header */
    char spare_pt[] = "01"; /* spare half-octet + payload-container type N1SM */
    char pc_len[] = "0047"; /* payload-container length = 0x47 (= 71)          */

    /* N1 SM payload: PDU Session Establishment Accept */
    char sm_epd[] = "2e";                                 /* EPD 0x2e (SM)            */
    char pdu_id[] = "01";                                 /* PDU Session ID           */
    char pti[] = "01";                                    /* Procedure Tx ID          */
    char sm_msg[] = "c2";                                 /* Establishment Accept     */
    char ssc_pdu[] = "11";                                /* SSC mode 1, IPv4         */
    char qos_rule[] = "000901000631310101ff01";           /* authorised QoS rule      */
    char ambr[] = "0603f42403f424";                       /* Session-AMBR 1 000 000 kbps */
    char pdu_addr[] = "2905010a2d0002";                   /* IPv4 address 10.45.0.2   */
    char snssai[] = "220101";                             /* SST = eMBB               */
    char qos_flow[] = "790006012041010109";               /* QoS flow 5QI = 9         */
    char epco[] = "7b000f80000d0408080808000d0408080404"; /* DNS 8.8.8.8 & 8.8.4.4    */
    char dnn[] = "250908696e7465726e6574";                /* DNN “internet”           */
    char pdu_id_ie[] = "1201";                            /* PDU Session ID (IE)      */

    char *parts[] = {outer_hdr, mac,     seq,      inner_hdr, dl_msg,   spare_pt, pc_len,   sm_epd, pdu_id, pti,
                     sm_msg,    ssc_pdu, qos_rule, ambr,      pdu_addr, snssai,   qos_flow, epco,   dnn,    pdu_id_ie};

    size_t off = 0;
    for (size_t i = 0; i < sizeof(parts) / sizeof(parts[0]); ++i)
    {
        size_t len = std::strlen(parts[i]);
        if (off + len >= MAX_NAS_HEX_LEN)
        {
            nas_payload_buf[0] = '\0';
            return nas_payload_buf;
        }
        std::memcpy(nas_payload_buf + off, parts[i], len);
        off += len;
    }
    nas_payload_buf[off] = '\0';
    return nas_payload_buf;
}

void NgapTask::deliverDownlinkNasRefactored()
{
    constexpr int hardcoded_ueId = 1;
    m_logger->debug("deliverDownlinkNas for UE[%d] times: %d", hardcoded_ueId, times);

    struct NasEntry
    {
        const char *(*generator)(); // Function returning NAS PDU in hex format
        const char *description;    // Human-readable description for logging
    };

    static const NasEntry entries[] = {{generate_auth_req, "Authentication Request"},
                                       {generate_security_cmd, "Security Command"},
                                       {generate_registration_accept, "Registration Accept"},
                                       {generate_configuration_update, "Configuration Update"},
                                       {generate_pdu_session_establishment, "PDU Session Establishment"}};

    const size_t entryCount = sizeof(entries) / sizeof(entries[0]);
    if (times > static_cast<int>(entryCount))
    {
        exit(1); // No more predefined PDUs – terminate as before
    }

    const NasEntry &entry = entries[times - 1];
    const char *hexPduC = entry.generator();
    std::string hexPdu(hexPduC); // create std::string view for FromHex helper
    m_logger->info("%s Ziyan", entry.description);

    // Increment the call counter
    times++;

    // Convert the selected hex string to OctetString
    OctetString pdu = OctetString::FromHex(hexPdu);

    // Print the NAS PDU content in hexadecimal format for debugging
    std::string hexString;
    char hex[3];
    for (size_t i = 0; i < pdu.length(); i++)
    {
        snprintf(hex, sizeof(hex), "%02x", static_cast<unsigned char>(pdu.data()[i]));
        hexString += hex;
    }
    m_logger->debug("NAS PDU content: %s", hexString.c_str());

    // Push message to RRC task
    auto w = std::make_unique<NmGnbNgapToRrc>(NmGnbNgapToRrc::NAS_DELIVERY);
    w->ueId = hardcoded_ueId;
    w->pdu = std::move(pdu);
    m_base->rrcTask->push(std::move(w));
}

int guard = 5;

void NgapTask::handleUplinkNasTransport(int ueId, const OctetString &nasPdu)
{
    auto *ue = findUeContext(ueId);
    if (ue == nullptr)
        return;

    auto *ieNasPdu = asn::New<ASN_NGAP_UplinkNASTransport_IEs>();
    ieNasPdu->id = ASN_NGAP_ProtocolIE_ID_id_NAS_PDU;
    ieNasPdu->criticality = ASN_NGAP_Criticality_reject;
    ieNasPdu->value.present = ASN_NGAP_UplinkNASTransport_IEs__value_PR_NAS_PDU;
    asn::SetOctetString(ieNasPdu->value.choice.NAS_PDU, nasPdu);

    auto *pdu = asn::ngap::NewMessagePdu<ASN_NGAP_UplinkNASTransport>({ieNasPdu});
    // sendNgapUeAssociated(ueId, pdu);

    if (times <= guard)
    {
        usleep(500000);
        deliverDownlinkNasRefactored();
    }
}

void NgapTask::sendNasNonDeliveryIndication(int ueId, const OctetString &nasPdu, NgapCause cause)
{
    m_logger->debug("Sending non-delivery indication for UE[%d]", ueId);

    auto *ieNasPdu = asn::New<ASN_NGAP_NASNonDeliveryIndication_IEs>();
    ieNasPdu->id = ASN_NGAP_ProtocolIE_ID_id_NAS_PDU;
    ieNasPdu->criticality = ASN_NGAP_Criticality_ignore;
    ieNasPdu->value.present = ASN_NGAP_NASNonDeliveryIndication_IEs__value_PR_NAS_PDU;
    asn::SetOctetString(ieNasPdu->value.choice.NAS_PDU, nasPdu);

    auto *ieCause = asn::New<ASN_NGAP_NASNonDeliveryIndication_IEs>();
    ieCause->id = ASN_NGAP_ProtocolIE_ID_id_Cause;
    ieCause->criticality = ASN_NGAP_Criticality_ignore;
    ieCause->value.present = ASN_NGAP_NASNonDeliveryIndication_IEs__value_PR_Cause;
    ngap_utils::ToCauseAsn_Ref(cause, ieCause->value.choice.Cause);

    auto *pdu = asn::ngap::NewMessagePdu<ASN_NGAP_NASNonDeliveryIndication>({ieNasPdu, ieCause});
    sendNgapUeAssociated(ueId, pdu);
}

// void NgapTask::receiveDownlinkNasTransport_backup(int amfId, ASN_NGAP_DownlinkNASTransport *msg)
// {
//     auto *ue = findUeByNgapIdPair(amfId, ngap_utils::FindNgapIdPair(msg));
//     if (ue == nullptr)
//         return;

//     auto *ieNasPdu = asn::ngap::GetProtocolIe(msg, ASN_NGAP_ProtocolIE_ID_id_NAS_PDU);
//     if (ieNasPdu)
//         deliverDownlinkNas(ue->ctxId, asn::GetOctetString(ieNasPdu->NAS_PDU));
// }

void NgapTask::receiveDownlinkNasTransport(int amfId, ASN_NGAP_DownlinkNASTransport *msg)
{
    NgapIdPair pair = NgapIdPair(1, 1);
    int hardcoded_amfId = 2;

    m_logger->info("amfId: %d, amfUeNgapId: %d, ranUeNgapId: %d", hardcoded_amfId, pair.amfUeNgapId, pair.ranUeNgapId);
    auto *ue = findUeByNgapIdPair(hardcoded_amfId, pair);
    if (ue == nullptr)
        return;

    auto *ieNasPdu = asn::ngap::GetProtocolIe(msg, ASN_NGAP_ProtocolIE_ID_id_NAS_PDU);

    if (times <= guard)
    {
        m_logger->info("bypass guard %d, times: %d", guard, times);
        return;
    }
    if (ieNasPdu)
        deliverDownlinkNas(ue->ctxId, asn::GetOctetString(ieNasPdu->NAS_PDU));
}

void NgapTask::receiveRerouteNasRequest(int amfId, ASN_NGAP_RerouteNASRequest *msg)
{
    m_logger->debug("Reroute NAS request received");

    auto *ue = findUeByNgapIdPair(amfId, ngap_utils::FindNgapIdPair(msg));
    if (ue == nullptr)
        return;

    auto *ieNgapMessage = asn::ngap::GetProtocolIe(msg, ASN_NGAP_ProtocolIE_ID_id_NGAP_Message);
    auto *ieAmfSetId = asn::ngap::GetProtocolIe(msg, ASN_NGAP_ProtocolIE_ID_id_AMFSetID);
    auto *ieAllowedNssai = asn::ngap::GetProtocolIe(msg, ASN_NGAP_ProtocolIE_ID_id_AllowedNSSAI);

    auto ngapPdu = asn::New<ASN_NGAP_NGAP_PDU>();
    ngapPdu->present = ASN_NGAP_NGAP_PDU_PR_initiatingMessage;
    ngapPdu->choice.initiatingMessage = asn::New<ASN_NGAP_InitiatingMessage>();
    ngapPdu->choice.initiatingMessage->procedureCode = ASN_NGAP_ProcedureCode_id_InitialUEMessage;
    ngapPdu->choice.initiatingMessage->criticality = ASN_NGAP_Criticality_ignore;
    ngapPdu->choice.initiatingMessage->value.present = ASN_NGAP_InitiatingMessage__value_PR_InitialUEMessage;

    auto *initialUeMessage = &ngapPdu->choice.initiatingMessage->value.choice.InitialUEMessage;

    if (!ngap_encode::DecodeInPlace(asn_DEF_ASN_NGAP_InitialUEMessage, ieNgapMessage->OCTET_STRING, &initialUeMessage))
    {
        m_logger->err("APER decoding failed in Reroute NAS Request");
        asn::Free(asn_DEF_ASN_NGAP_NGAP_PDU, ngapPdu);
        sendErrorIndication(amfId, NgapCause::Protocol_transfer_syntax_error);
        return;
    }

    if (ieAllowedNssai)
    {
        auto *oldAllowedNssai = asn::ngap::GetProtocolIe(initialUeMessage, ASN_NGAP_ProtocolIE_ID_id_AllowedNSSAI);
        if (oldAllowedNssai)
            asn::DeepCopy(asn_DEF_ASN_NGAP_AllowedNSSAI, ieAllowedNssai->AllowedNSSAI, &oldAllowedNssai->AllowedNSSAI);
        else
        {
            auto *newAllowedNssai = asn::New<ASN_NGAP_InitialUEMessage_IEs>();
            newAllowedNssai->id = ASN_NGAP_ProtocolIE_ID_id_AllowedNSSAI;
            newAllowedNssai->criticality = ASN_NGAP_Criticality_reject;
            newAllowedNssai->value.present = ASN_NGAP_InitialUEMessage_IEs__value_PR_AllowedNSSAI;

            asn::ngap::AddProtocolIe(*initialUeMessage, newAllowedNssai);
        }
    }

    auto *newAmf = selectNewAmfForReAllocation(ue->ctxId, amfId, asn::GetBitStringInt<10>(ieAmfSetId->AMFSetID));
    if (newAmf == nullptr)
    {
        m_logger->err("AMF selection for re-allocation failed. Could not find a suitable AMF.");
        return;
    }

    sendNgapUeAssociated(ue->ctxId, ngapPdu);
}

} // namespace nr::gnb
