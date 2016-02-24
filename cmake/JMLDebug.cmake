include(JMLColors)
macro(JML_DEBUG_MESSAGE _messageText)
    set(COLUMNS $ENV{COLUMNS})
    if(NOT COLUMNS)
        set(COLUMNS 80)
    endif()

    string(RANDOM LENGTH ${COLUMNS} ALPHABET "=" SEPARATOR_MESSAGE)

    message("${Green}${SEPARATOR_MESSAGE}${ColorReset}")
    message(STATUS "${Blue}${_messageText}${ColorReset}")
    message("${Green}${SEPARATOR_MESSAGE}${ColorReset}")
endmacro(JML_DEBUG_MESSAGE)
