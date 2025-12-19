# ============================================================================
# MCP Qt Settings - Global Qt compile definitions
# Applies Qt-specific compile definitions globally for stricter type safety.
# Disables Qt keywords and enforces explicit type conversions.
# ============================================================================

add_definitions(
    -DQT_NO_KEYWORDS
    -DQT_NO_CAST_FROM_BYTEARRAY
    -DQT_NO_CAST_FROM_ASCII
    -DQT_NO_CAST_TO_ASCII
)
