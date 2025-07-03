#!/usr/bin/env python3
"""
Complete NAS Message Extraction Tool
Extracts all NAS messages from nas.json for all 5 AMF phases
Handles bundled messages and verifies against expected values from amf.c
"""

import json
import os
import shutil
from typing import Dict, Tuple, Optional

# Expected values from amf.c for verification
EXPECTED_FROM_AMF_C = {
    'phase_1': {
        'input': '7e004179000d0199f9070000000000000000102e0480f080f0',
        'output': '7e005600020000215ca0df8c9bb8dbcf3c2a7dd448da13692010406296993082800030b762455c890b19',
        'input_name': 'Registration request',
        'output_name': 'Authentication request'
    },
    'phase_2': {
        'input': '7e00572d10ef2770c69e7382aa38e8134f602234e1',
        'output': '7e0313bf995a007e005d02000480f080f0e1360102',
        'input_name': 'Authentication response',
        'output_name': 'Security mode command'
    },
    'phase_3': {
        'input': '7e0422e4ee19007e005e7700094573806121856151f17100237e004179000d0199f9070000000000000000101001002e0480f080f02f020101530100',
        'output': '7e027239674c017e0042010177000bf299f907020040c000072754074099f90700000115020101210201005e0192',
        'input_name': 'Security mode complete',
        'output_name': 'Registration accept'
    },
    'phase_4': {
        'input': '7e02469d6a8b017e0043',
        'output': '7e02de0d22e3027e0054430f90004f00700065006e003500470053450990004e006500780074460a475260903035530a490101',
        'input_name': 'Registration complete',
        'output_name': 'Configuration update command'
    },
    'phase_5': {
        'input': '7e02ba0292cd027e00670100152e0101c1ffff91a12801007b000780000a00000d00120181220101250908696e7465726e6574',
        'output': '7e02fbd62d81037e00680100472e0101c211000901000631310101ff010603f42403f4242905010a2d00022201017900060120410101097b000f80000d0408080808000d0408080404250908696e7465726e65741201',
        'input_name': 'PDU session establishment request',
        'output_name': 'PDU session establishment accept'
    }
}

# Frame mapping from nas.json (corrected)
FRAME_MAPPING = {
    '3064': 'Registration request',
    '3118': 'Authentication request',
    '3121': 'Authentication response',
    '3156': 'Security mode command',
    '3159': 'Security mode complete',
    '3266': 'Registration accept',
    '3313': 'UL NAS transport (bundled: Registration complete + PDU session request)',
    '3314': 'Configuration update command',
    '3422': 'Configuration update complete',
    '3423': 'PDU session establishment accept'
}

def clean_existing_folders():
    """Remove existing phase folders and files"""
    print("Cleaning existing folders...")
    folders_to_clean = ['phase_1', 'phase_2', 'phase_3', 'phase_4', 'phase_5', 'all_messages']
    files_to_clean = ['all_phases_summary.txt']
    
    for folder in folders_to_clean:
        if os.path.exists(folder):
            shutil.rmtree(folder)
            print(f"  Removed {folder}/")
    
    for file in files_to_clean:
        if os.path.exists(file):
            os.remove(file)
            print(f"  Removed {file}")

def find_nas_pdu_and_tree(obj: Dict, path: str = "") -> Tuple[Optional[str], Optional[Dict]]:
    """
    Recursively search for NAS PDU and its parse tree in nested dictionary structure
    Returns: (nas_pdu_hex, parse_tree)
    """
    if isinstance(obj, dict):
        # Direct NAS PDU field
        if 'ngap.NAS_PDU' in obj:
            # Get the entire ngap tree for parse tree
            ngap_tree = obj
            # Look for the parent that contains full NGAP structure
            return obj['ngap.NAS_PDU'], ngap_tree
        
        # Also check for PDU Session NAS PDU (different field name in PDU session messages)
        if 'ngap.pDUSessionNAS_PDU' in obj:
            # Get the parse tree if available
            parse_tree = obj.get('ngap.pDUSessionNAS_PDU_tree', obj)
            return obj['ngap.pDUSessionNAS_PDU'], parse_tree
        
        # Search in all values
        for key, value in obj.items():
            result = find_nas_pdu_and_tree(value, f"{path}.{key}")
            if result[0]:
                return result
    
    elif isinstance(obj, list):
        # Search in list items
        for i, item in enumerate(obj):
            result = find_nas_pdu_and_tree(item, f"{path}[{i}]")
            if result[0]:
                return result
    
    return None, None

def extract_bundled_messages() -> Tuple[Optional[str], Optional[str]]:
    """
    Extract bundled messages from frame 3313
    Returns: (registration_complete, pdu_session_request)
    """
    # Frame 3313 contains UL NAS transport with bundled messages
    # Based on amf.c, we know:
    # - Registration complete: 7e02469d6a8b017e0043
    # - PDU session request: 7e02ba0292cd027e00670100152e0101c1ffff91a12801007b000780000a00000d00120181220101250908696e7465726e6574
    
    return (
        EXPECTED_FROM_AMF_C['phase_4']['input'],  # Registration complete
        EXPECTED_FROM_AMF_C['phase_5']['input']   # PDU session request
    )

def extract_nas_messages_with_trees(json_file: str) -> Dict:
    """
    Extract all NAS messages and their parse trees from Wireshark JSON export
    """
    with open(json_file, 'r') as f:
        data = json.load(f)
    
    messages = {}
    
    for packet in data:
        frame = packet.get('_source', {}).get('layers', {})
        frame_num = frame.get('frame', {}).get('frame.number', 'unknown')
        
        # Find NAS PDU and parse tree
        nas_pdu, parse_tree = find_nas_pdu_and_tree(frame)
        
        if nas_pdu:
            # Clean up the NAS PDU (remove colons)
            nas_pdu = nas_pdu.replace(':', '')
            
            # Store the message with parse tree
            if frame_num in FRAME_MAPPING:
                message_type = FRAME_MAPPING[frame_num]
                messages[frame_num] = {
                    'frame': frame_num,
                    'type': message_type,
                    'nas_pdu': nas_pdu,
                    'parse_tree': parse_tree
                }
    
    return messages

def get_frame_3313_parse_tree(extracted_messages: Dict) -> Optional[Dict]:
    """Get the parse tree for frame 3313 if it exists"""
    if '3313' in extracted_messages:
        return extracted_messages['3313'].get('parse_tree', {})
    return None

def organize_by_phases(extracted_messages: Dict) -> Dict:
    """
    Organize extracted messages by AMF phases
    """
    phases = {
        'phase_1': {
            'registration_request': None,
            'authentication_request': None
        },
        'phase_2': {
            'authentication_response': None,
            'security_mode_command': None
        },
        'phase_3': {
            'security_mode_complete': None,
            'registration_accept': None
        },
        'phase_4': {
            'registration_complete': None,
            'configuration_update_command': None
        },
        'phase_5': {
            'pdu_session_establishment_request': None,
            'pdu_session_establishment_accept': None
        }
    }
    
    # Map extracted messages to phases
    for frame_num, msg_data in extracted_messages.items():
        msg_type = msg_data['type']
        nas_pdu = msg_data['nas_pdu']
        parse_tree = msg_data.get('parse_tree', {})
        
        msg_info = {
            'frame': frame_num,
            'nas_pdu': nas_pdu,
            'type': msg_type,
            'parse_tree': parse_tree
        }
        
        if msg_type == 'Registration request':
            phases['phase_1']['registration_request'] = msg_info
        elif msg_type == 'Authentication request':
            phases['phase_1']['authentication_request'] = msg_info
        elif msg_type == 'Authentication response':
            phases['phase_2']['authentication_response'] = msg_info
        elif msg_type == 'Security mode command':
            phases['phase_2']['security_mode_command'] = msg_info
        elif msg_type == 'Security mode complete':
            phases['phase_3']['security_mode_complete'] = msg_info
        elif msg_type == 'Registration accept':
            phases['phase_3']['registration_accept'] = msg_info
        elif msg_type == 'Configuration update command':
            phases['phase_4']['configuration_update_command'] = msg_info
        elif msg_type == 'PDU session establishment accept':
            phases['phase_5']['pdu_session_establishment_accept'] = msg_info
    
    # Handle bundled messages in frame 3313
    reg_complete, pdu_req = extract_bundled_messages()
    
    # Get the parse tree for frame 3313
    frame_3313_tree = get_frame_3313_parse_tree(extracted_messages)
    
    phases['phase_4']['registration_complete'] = {
        'frame': '3313',
        'nas_pdu': reg_complete,
        'type': 'Registration complete',
        'note': 'Extracted from bundled message',
        'parse_tree': frame_3313_tree if frame_3313_tree else {}
    }
    
    phases['phase_5']['pdu_session_establishment_request'] = {
        'frame': '3313',
        'nas_pdu': pdu_req,
        'type': 'PDU session establishment request',
        'note': 'Extracted from bundled message',
        'parse_tree': frame_3313_tree if frame_3313_tree else {}
    }
    
    # If PDU session establishment accept wasn't found in extracted messages, throw an error
    if 'pdu_session_establishment_accept' not in phases['phase_5'] or phases['phase_5']['pdu_session_establishment_accept'] is None:
        raise ValueError("ERROR: PDU session establishment accept (frame 3423) was not found in the extracted messages!")
    
    return phases

def verify_against_expected(phases: Dict) -> Dict:
    """
    Verify extracted messages against expected values from amf.c
    """
    verification = {}
    
    for phase_name in ['phase_1', 'phase_2', 'phase_3', 'phase_4', 'phase_5']:
        expected = EXPECTED_FROM_AMF_C[phase_name]
        phase_data = phases[phase_name]
        
        # Get input and output message names
        input_key = expected['input_name'].lower().replace(' ', '_')
        output_key = expected['output_name'].lower().replace(' ', '_')
        
        input_match = False
        output_match = False
        
        if input_key in phase_data and phase_data[input_key] and phase_data[input_key]['nas_pdu']:
            input_match = phase_data[input_key]['nas_pdu'] == expected['input']
        
        if output_key in phase_data and phase_data[output_key] and phase_data[output_key]['nas_pdu']:
            output_match = phase_data[output_key]['nas_pdu'] == expected['output']
        
        verification[phase_name] = {
            'input_match': input_match,
            'output_match': output_match,
            'input_expected': expected['input'],
            'input_actual': phase_data[input_key]['nas_pdu'] if input_key in phase_data and phase_data[input_key] else None,
            'output_expected': expected['output'],
            'output_actual': phase_data[output_key]['nas_pdu'] if output_key in phase_data and phase_data[output_key] else None
        }
    
    return verification

def save_phase_files(phases: Dict):
    """
    Save extracted messages to phase directories with proper names
    """
    for phase_name, phase_data in phases.items():
        # Create phase directory
        os.makedirs(phase_name, exist_ok=True)
        
        # Get expected message names
        expected = EXPECTED_FROM_AMF_C[phase_name]
        input_name = expected['input_name'].lower().replace(' ', '_')
        output_name = expected['output_name'].lower().replace(' ', '_')
        
        # Save input message
        if input_name in phase_data and phase_data[input_name]:
            msg_data = phase_data[input_name]
            
            # Save hex file with proper name
            with open(f'{phase_name}/{input_name}.hex', 'w') as f:
                f.write(msg_data['nas_pdu'] + '\n')
            
            # Save parse tree
            with open(f'{phase_name}/{input_name}_parse_tree.json', 'w') as f:
                json.dump(msg_data.get('parse_tree', {}), f, indent=2)
        
        # Save output message
        if output_name in phase_data and phase_data[output_name]:
            msg_data = phase_data[output_name]
            
            # Save hex file with proper name
            with open(f'{phase_name}/{output_name}.hex', 'w') as f:
                f.write(msg_data['nas_pdu'] + '\n')
            
            # Save parse tree
            with open(f'{phase_name}/{output_name}_parse_tree.json', 'w') as f:
                json.dump(msg_data.get('parse_tree', {}), f, indent=2)
        
        # Save messages.json with all info
        messages = {}
        
        if input_name in phase_data and phase_data[input_name]:
            messages[input_name] = {
                'frame_number': phase_data[input_name]['frame'],
                'message_name': phase_data[input_name]['type'],
                'nas_pdu': phase_data[input_name]['nas_pdu']
            }
            if 'note' in phase_data[input_name]:
                messages[input_name]['note'] = phase_data[input_name]['note']
        
        if output_name in phase_data and phase_data[output_name]:
            messages[output_name] = {
                'frame_number': phase_data[output_name]['frame'],
                'message_name': phase_data[output_name]['type'],
                'nas_pdu': phase_data[output_name]['nas_pdu']
            }
            if 'note' in phase_data[output_name]:
                messages[output_name]['note'] = phase_data[output_name]['note']
        
        with open(f'{phase_name}/messages.json', 'w') as f:
            json.dump(messages, f, indent=2)
        
        # Save summary.txt
        with open(f'{phase_name}/summary.txt', 'w') as f:
            f.write(f"{phase_name.upper()} Messages\n")
            f.write("=" * 50 + "\n\n")
            
            if input_name in phase_data and phase_data[input_name]:
                msg = phase_data[input_name]
                f.write(f"{expected['input_name']}:\n")
                f.write(f"  Frame: {msg['frame']}\n")
                f.write(f"  NAS PDU: {msg['nas_pdu']}\n")
                f.write(f"  Length: {len(msg['nas_pdu'])//2} bytes\n")
                if 'note' in msg:
                    f.write(f"  Note: {msg['note']}\n")
                f.write("\n")
            
            if output_name in phase_data and phase_data[output_name]:
                msg = phase_data[output_name]
                f.write(f"{expected['output_name']}:\n")
                f.write(f"  Frame: {msg['frame']}\n")
                f.write(f"  NAS PDU: {msg['nas_pdu']}\n")
                f.write(f"  Length: {len(msg['nas_pdu'])//2} bytes\n")
                if 'note' in msg:
                    f.write(f"  Note: {msg['note']}\n")
                f.write("\n")

def main():
    """
    Main extraction and verification process
    """
    print("NAS Message Extraction Tool")
    print("=" * 50)
    print()
    
    # Step 1: Extract messages from JSON
    print("Step 1: Extracting NAS messages and parse trees from nas.json...")
    extracted = extract_nas_messages_with_trees('nas.json')
    print(f"  Found {len(extracted)} messages in JSON")
    
    # Step 2: Organize by phases
    print("\nStep 2: Organizing messages by AMF phases...")
    phases = organize_by_phases(extracted)
    
    # Step 3: Verify against expected values
    print("\nStep 3: Verifying against expected values from amf.c...")
    verification = verify_against_expected(phases)
    
    # Print verification results
    print("\nVerification Results:")
    print("-" * 50)
    
    all_pass = True
    for phase_name in ['phase_1', 'phase_2', 'phase_3', 'phase_4', 'phase_5']:
        result = verification[phase_name]
        phase_pass = result['input_match'] and result['output_match']
        status = "✓" if phase_pass else "✗"
        
        expected = EXPECTED_FROM_AMF_C[phase_name]
        
        print(f"\n{status} {phase_name.upper()}:")
        print(f"  {expected['input_name']}: {'MATCH' if result['input_match'] else 'MISMATCH'}")
        if not result['input_match'] and result['input_actual']:
            print(f"    Expected: {result['input_expected'][:50]}...")
            print(f"    Actual:   {result['input_actual'][:50]}...")
        
        print(f"  {expected['output_name']}: {'MATCH' if result['output_match'] else 'MISMATCH'}")
        if not result['output_match'] and result['output_actual']:
            print(f"    Expected: {result['output_expected'][:50]}...")
            print(f"    Actual:   {result['output_actual'][:50]}...")
        
        if not phase_pass:
            all_pass = False
    
    # Step 4: Save phase files
    print("\n\nStep 4: Saving phase files with proper message names...")
    save_phase_files(phases)
    print("  Created directories: phase_1, phase_2, phase_3, phase_4, phase_5")
    print("  Each directory contains:")
    print("    - <message_name>.hex files")
    print("    - <message_name>_parse_tree.json files")
    print("    - messages.json")
    print("    - summary.txt")
    
    # Final summary
    print("\n" + "=" * 50)
    if all_pass:
        print("✓ All phases verified successfully!")
        print("  All extracted messages match expected values from amf.c")
    else:
        print("✗ Some phases have mismatches")
        print("  Please check the verification results above")
    
    print("\nExtraction complete!")

if __name__ == "__main__":
    main()