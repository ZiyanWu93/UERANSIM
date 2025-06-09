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
    int hardcoded_ueId = 1;
    m_logger->debug("deliverDownlinkNas for UE[%d] times: %d", hardcoded_ueId, times);

    // Select the hardcoded PDU based on call count
    std::string hexPdu;
    switch (times)
    {
    case 1:
        hexPdu = "7e005600020000215ca0df8c9bb8dbcf3c2a7dd448da13692010406296993082800030b762455c890b19";
        break;
    case 2:
        hexPdu = "7e0313bf995a007e005d02000480f080f0e1360102";
        break;
    case 3:
        hexPdu = "7e027239674c017e0042010177000bf299f907020040c000072754074099f90700000115020101210201005e0192";
        break;
    case 4:
        hexPdu =
            "7e02de0d22e3027e0054430f90004f00700065006e003500470053450990004e006500780074460a475260903035530a490101";
        break;
    case 5:
        hexPdu = "7e02fbd62d81037e00680100472e0101c211000901000631310101ff010603f42403f4242905010a2d0002220101790006012"
                 "0410101097b000f80000d0408080808000d0408080404250908696e7465726e65741201";
        break;
    default:
        exit(1); // Exit the program
    }
    // Increment the call counter
    times++;

    // Convert the selected hex string to OctetString
    OctetString pdu = OctetString::FromHex(hexPdu);

    // Print the NAS PDU content in hexadecimal format
    // Print the NAS PDU content in hexadecimal format
    std::string hexString;
    char hex[3];
    for (size_t i = 0; i < pdu.length(); i++)
    {
        snprintf(hex, sizeof(hex), "%02x", static_cast<unsigned char>(pdu.data()[i]));
        hexString += hex;
    }
    m_logger->debug("NAS PDU content: %s", hexString.c_str());

    auto w = std::make_unique<NmGnbNgapToRrc>(NmGnbNgapToRrc::NAS_DELIVERY);
    w->ueId = hardcoded_ueId;
    w->pdu = std::move(pdu);
    m_base->rrcTask->push(std::move(w));
}

void NgapTask::deliverDownlinkNasRefactored()
{
    int hardcoded_ueId = 1;
    m_logger->debug("deliverDownlinkNas for UE[%d] times: %d", hardcoded_ueId, times);

    // Select the hardcoded PDU based on call count
    std::string hexPdu;
    switch (times)
    {
    case 1:
        hexPdu = "7e005600020000215ca0df8c9bb8dbcf3c2a7dd448da13692010406296993082800030b762455c890b19";
        m_logger->info("Authentication Request Ziyan");
        break;
    case 2:
        hexPdu = "7e0313bf995a007e005d02000480f080f0e1360102";
        m_logger->info("Security Command Ziyan");
        break;
    case 3:
        hexPdu = "7e027239674c017e0042010177000bf299f907020040c000072754074099f90700000115020101210201005e0192";
        m_logger->info("Registration Accept Ziyan");
        break;
    case 4:
        hexPdu =
            "7e02de0d22e3027e0054430f90004f00700065006e003500470053450990004e006500780074460a475260903035530a490101";
        m_logger->info("Configuration Update Ziyan");
        break;
    case 5:
        hexPdu = "7e02fbd62d81037e00680100472e0101c211000901000631310101ff010603f42403f4242905010a2d0002220101790006012"
                 "0410101097b000f80000d0408080808000d0408080404250908696e7465726e65741201";
        m_logger->info("PDU Session Establishment Ziyan");

        break;
    default:
        exit(1); // Exit the program
    }
    // Increment the call counter
    times++;

    // Convert the selected hex string to OctetString
    OctetString pdu = OctetString::FromHex(hexPdu);

    // Print the NAS PDU content in hexadecimal format
    // Print the NAS PDU content in hexadecimal format
    std::string hexString;
    char hex[3];
    for (size_t i = 0; i < pdu.length(); i++)
    {
        snprintf(hex, sizeof(hex), "%02x", static_cast<unsigned char>(pdu.data()[i]));
        hexString += hex;
    }
    m_logger->debug("NAS PDU content: %s", hexString.c_str());

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
