#  install manual and examples

if(UNIX)
    install(DIRECTORY examples DESTINATION share/qmolview/
            FILES_MATCHING PATTERN "*.xyz"
            PATTERN "*.cml"
            PATTERN "*.molden_normal_modes")

    install(FILES doc/qmolviewmanual.pdf  DESTINATION  share/qmolview/doc)
endif()
