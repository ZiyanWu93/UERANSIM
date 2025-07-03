# Field Coverage Analysis: registration_accept_parse_tree.json vs ROADMAP-phase3.md

## Complete Field List from registration_accept_parse_tree.json

### Top-level fields
1. **per.octet_string_length: "46"** - ❌ NOT COVERED
2. **ngap.NAS_PDU: (hex string)** - ❌ NOT COVERED explicitly (encoding format)
3. **ngap.NAS_PDU_tree** - ✓ Partially covered (as parse tree structure)

### Security Header Fields
4. **nas_5gs.epd: "126"** - ✓ COVERED (line 48, 87)
5. **nas_5gs.spare_half_octet: "0"** - ❌ NOT COVERED
6. **nas_5gs.security_header_type: "2"** - ✓ COVERED (line 49, 88)
7. **nas_5gs.msg_auth_code: "0x7239674c"** - ✓ COVERED (line 50, 94)
8. **nas_5gs.seq_no: "1"** - ✓ COVERED (line 51, 93)

### Plain NAS Message Fields
9. **nas_5gs.epd: "126"** (repeated) - ✓ COVERED
10. **nas_5gs.spare_half_octet: "0"** (repeated) - ❌ NOT COVERED
11. **nas_5gs.security_header_type: "0"** - ❌ NOT COVERED (plain message security header)
12. **nas_5gs.mm.message_type: "0x42"** - ✓ COVERED (line 52, 89)

### 5GS Registration Result
13. **gsm_a.len: "1"** - ❌ NOT COVERED (length field)
14. **nas_5gs.mm.reg_res.emergency_reg: "0"** - ❌ NOT COVERED
15. **nas_5gs.mm.reg_res.nssaa_perf: "0"** - ❌ NOT COVERED
16. **nas_5gs.mm.reg_res.sms_all: "0"** - ❌ NOT COVERED
17. **nas_5gs.mm.reg_res.res: "1"** - ✓ COVERED (line 54, 90)

### 5GS Mobile Identity - 5G-GUTI
18. **nas_5gs.mm.elem_id: "0x77"** - ❌ NOT COVERED (element ID)
19. **gsm_a.len: "11"** - ❌ NOT COVERED (length field)
20. **nas_5gs.spare_b7 to spare_b3** - ❌ NOT COVERED (spare bits)
21. **nas_5gs.mm.type_id: "2"** - ✓ COVERED (line 57, 96)
22. **e212.guami.mcc: "999"** - ✓ COVERED (line 58, 97)
23. **e212.guami.mnc: "70"** - ✓ COVERED (line 59, 97)
24. **nas_5gs.amf_region_id: "2"** - ✓ COVERED (line 60, 99)
25. **nas_5gs.amf_set_id: "1"** - ✓ COVERED (line 61, 100)
26. **nas_5gs.amf_pointer: "0"** - ✓ COVERED (line 62, 101)
27. **nas_5gs.5g_tmsi: "3221227303"** - ✓ COVERED (line 63, 102)
28. **3gpp.tmsi: "3221227303"** - ❌ NOT COVERED (duplicate field)

### 5GS Tracking Area Identity List
29. **nas_5gs.mm.elem_id: "0x54"** - ❌ NOT COVERED (element ID)
30. **gsm_a.len: "7"** - ❌ NOT COVERED (length field)
31. **nas_5gs.mm.tal_t_li: "2"** - ✓ COVERED (line 66, 259)
32. **nas_5gs.mm.tal_num_e: "0"** - ❌ NOT COVERED (number of elements)
33. **e212.5gstai.mcc: "999"** - ✓ COVERED (line 67)
34. **e212.5gstai.mnc: "70"** - ✓ COVERED (line 68)
35. **nas_5gs.tac: "1"** - ✓ COVERED (line 69)

### NSSAI - Allowed NSSAI
36. **nas_5gs.mm.elem_id: "0x15"** - ❌ NOT COVERED (element ID)
37. **gsm_a.len: "2"** - ❌ NOT COVERED (length field)
38. **nas_5gs.mm.length: "1"** - ❌ NOT COVERED (S-NSSAI length)
39. **nas_5gs.mm.sst: "1"** - ✓ COVERED (line 72)

### 5GS Network Feature Support
40. **nas_5gs.mm.elem_id: "0x21"** - ❌ NOT COVERED (element ID)
41. **gsm_a.len: "2"** - ❌ NOT COVERED (length field)
42. **nas_5gs.nw_feat_sup.mpsi: "0"** - ❌ NOT COVERED
43. **nas_5gs.nw_feat_sup.iwk_n26: "0"** - ❌ NOT COVERED (mentioned in bitmap but not explicit)
44. **nas_5gs.nw_feat_sup.emf: "0"** - ❌ NOT COVERED
45. **nas_5gs.nw_feat_sup.emc: "0"** - ❌ NOT COVERED (mentioned as emergency service)
46. **nas_5gs.nw_feat_sup.vops_n3gpp: "0"** - ❌ NOT COVERED (mentioned in bitmap)
47. **nas_5gs.nw_feat_sup.vops_3gpp: "1"** - ✓ COVERED (line 75)
48. **nas_5gs.nw_feat_sup.5g_ciot_up: "0"** - ❌ NOT COVERED
49. **nas_5gs.nw_feat_sup.5g_iphc_cp_ciot: "0"** - ❌ NOT COVERED
50. **nas_5gs.nw_feat_sup.n3_data: "0"** - ❌ NOT COVERED
51. **nas_5gs.nw_feat_sup.5g_cp_ciot: "0"** - ❌ NOT COVERED
52. **nas_5gs.nw_feat_sup.restrict_ec: "0"** - ❌ NOT COVERED
53. **nas_5gs.nw_feat_sup.mcsi: "0"** - ❌ NOT COVERED
54. **nas_5gs.nw_feat_sup.emcn3: "0"** - ❌ NOT COVERED

### GPRS Timer 3 - T3512
55. **gsm_a.gm.elem_id: "0x5e"** - ❌ NOT COVERED (element ID)
56. **gsm_a.len: "1"** - ❌ NOT COVERED (length field)
57. **gsm_a.gm.gmm.gprs_timer3: "0x1c"** - ❌ NOT COVERED (raw encoded value)
58. **gsm_a.gm.gmm.gprs_timer3_unit: "4"** - ✓ COVERED (line 78)
59. **gsm_a.gm.gmm.gprs_timer3_value: "18"** - ✓ COVERED (line 79)

## Summary of Missing Coverage

### Critical Missing Fields:
1. **per.octet_string_length** - PER encoding length
2. **ngap.NAS_PDU** - Raw hex encoding format
3. **Element IDs** (0x77, 0x54, 0x15, 0x21, 0x5e) - IE identifiers
4. **Length fields** (gsm_a.len) - IE length encoding
5. **Spare bits/octets** - Padding and reserved fields

### Optional/Detailed Fields Not Covered:
1. Registration result sub-fields (emergency_reg, nssaa_perf, sms_all)
2. TAI list number of elements (tal_num_e)
3. Network feature support detailed bits (14 sub-fields)
4. Raw encoded timer value (0x1c)
5. Plain message security header type

### Recommendations:
1. Add section on NGAP PDU encoding including PER length
2. Document all IE element IDs and their meaning
3. Explain length field calculation for each IE
4. Cover spare bits/octets and their purpose
5. Detail all network feature support bits
6. Add raw encoding examples for timer values
7. Explain the dual security header (protected + plain)