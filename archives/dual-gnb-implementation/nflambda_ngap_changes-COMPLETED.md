# NFLambda NGAP Setup Changes (Phase 2)

This document details the specific changes related to NGAP setup between gNB and AMF for the NFLambda version.

## NGAP Setup Flow Changes

### 1. SCTP Connection Disabled
**File:** `src/gnb/sctp/task.cpp`
```cpp
// Line 122: Early return disables all SCTP processing
void SctpTask::onLoop() {
    return;  // <-- NFLambda: Bypass SCTP completely
    // ... rest of original SCTP handling
}
```

### 2. NGAP Setup Request Bypassed
**File:** `src/gnb/ngap/interface.cpp`
```cpp
// Line 174-180: Original NGAP setup request creation commented out
void NgapTask::sendNgSetupRequest(int amfId) {
    // NFLambda: Skip actual NGAP message creation and SCTP send
    // auto *pdu = asn::ngap::NewMessagePdu<ASN_NGAP_NGSetupRequest>(...);
    // sendNgapNonUe(amfId, pdu);
    
    // NFLambda: Directly trigger simulated response
    receiveNgSetupResponse_base_case();
}
```

### 3. Simulated NGAP Setup Response
**File:** `src/gnb/ngap/interface.cpp`
```cpp
// Lines 184-244: Hardcoded AMF configuration
void NgapTask::receiveNgSetupResponse_base_case() {
    // NFLambda: Simulate AMF response with hardcoded values
    m_amfCtx[2].amfId = 2;
    m_amfCtx[2].address = "127.0.0.5";
    m_amfCtx[2].port = 38412;
    m_amfCtx[2].state = EAmfState::CONNECTED;
    m_amfCtx[2].amfName = "open5gs-amf0";
    
    // Hardcoded GUAMI
    m_amfCtx[2].guami.mcc = 999;
    m_amfCtx[2].guami.mnc = 0;
    m_amfCtx[2].guami.amfRegionId = Octet{2};
    m_amfCtx[2].guami.amfSetId = BitString::Spare(10); // value: 1
    m_amfCtx[2].guami.amfPointer = BitString::Spare(6); // value: 0
    
    // Hardcoded PLMN support
    auto ieAmfTnlNgapLayer = new ASN_NGAP_AMF_TNLAssociationSetupItem;
    ieAmfTnlNgapLayer->aMF_TNLAssociationAddress.present = 
        ASN_NGAP_CPTransportLayerInformation_PR_endpointIPAddress;
    // ... more hardcoded values
}
```

### 4. NGAP Transport Handler Bypass
**File:** `src/gnb/ngap/transport.cpp`
```cpp
// Line 309: Bypass actual NGAP message handling
void NgapTask::handleSCTPMessage(...) {
    // ... message parsing ...
    
    // NFLambda: Skip processing real NGAP responses
    m_logger->info("Bypass 5G Core Response");
    return;  // <-- Exit before handling real NGAP messages
    
    // Original code would process NGSetupResponse here
}
```

### 5. Initialization Override
**File:** `src/gnb/ngap/task.cpp`
```cpp
// Lines 48-52: Force simulated NGAP setup on startup
void NgapTask::onLoop() {
    static bool first_time = true;
    if (first_time) {
        first_time = false;
        receiveNgSetupResponse_base_case();  // NFLambda: Simulate AMF connection
    }
    // ... rest of task loop
}
```

## Summary of NGAP Setup Changes

The NFLambda version completely bypasses the standard NGAP setup procedure:

1. **No SCTP Connection**: The SCTP layer is disabled, preventing any network communication
2. **No NGAP Messages**: NGSetupRequest is never sent over the wire
3. **Simulated AMF**: A fake AMF context is created with hardcoded values
4. **Automatic Setup**: The gNB automatically considers itself connected to AMF on startup

### Hardcoded Configuration Values
- AMF ID: `2`
- AMF Address: `127.0.0.5:38412`
- AMF Name: `open5gs-amf0`
- PLMN: MCC=`999`, MNC=`0`
- GUAMI: Region=`2`, Set=`1`, Pointer=`0`
- SCTP Streams: `10` (in/out)

## Restoration Requirements

To restore NGAP setup for the SCTP version:
1. Re-enable SCTP processing in `sctp/task.cpp`
2. Restore actual NGSetupRequest sending in `interface.cpp`
3. Remove automatic setup trigger in `task.cpp`
4. Restore NGAP message handling in `transport.cpp`
5. Make simulated response conditional with `#ifdef USE_NFLAMBDA`