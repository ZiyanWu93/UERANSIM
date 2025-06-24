# Core5G Documentation Update Summary

## Changes Made

### 1. Restructured Documentation
- Created new `README.md` as main entry point (replacing `overview.md`)
- Created `docs/` directory for detailed documentation:
  - `architecture.md` - System design overview
  - `api-reference.md` - Consolidated API documentation
  - `integration.md` - Guide for building applications
  - `examples.md` - Code examples and patterns

### 2. Simplified Component READMEs
- **Runtime**: Reduced from 520 to 117 lines (77% reduction)
- **Event System**: Reduced from 410 to 104 lines (75% reduction)
- **Mailbox**: Reduced from ~300 to 99 lines (67% reduction)
- **Memory**: Reduced from ~300 to 108 lines (64% reduction)

### 3. Applied DRY Principles
- Moved detailed API docs to central `api-reference.md`
- Eliminated duplicate architecture descriptions
- Consolidated examples in `examples.md`
- Removed redundant component descriptions

### 4. Updated Content
- Fixed references to non-existent `actor/` directory
- Added references to new AMF demo application
- Updated dependency management documentation
- Simplified AMF demo README

### 5. Improved Navigation
- Clear hierarchy: Overview → Components → Details
- Consistent linking between documents
- Practical examples referenced throughout

## Benefits

1. **Maintainability**: Single source of truth for APIs and examples
2. **Readability**: Focused, concise documentation
3. **Accuracy**: Reflects actual codebase structure
4. **Usability**: Clear navigation and practical examples
5. **Size**: ~60% reduction in total documentation size

## Document Structure

```
core5g/
├── README.md                    # Main entry (concise overview)
├── docs/                        # Detailed documentation
│   ├── architecture.md         # System design
│   ├── api-reference.md        # All APIs in one place
│   ├── integration.md          # How to build apps
│   └── examples.md             # Code patterns
├── runtime/README.md           # Brief component overview
├── event_system/README.md      # Brief component overview
├── mailbox/README.md           # Brief component overview
├── memory/README.md            # Brief component overview
└── app/
    ├── ping_pong/README.md     # Example app docs
    └── amf/README.md           # Example app docs
```