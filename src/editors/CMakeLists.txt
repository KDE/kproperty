ecm_generate_headers(kproperty_editors_FORWARDING_HEADERS
    # TODO: rename to camelcase, then uncomment "ORIGINAL CAMELCASE"
    HEADER_NAMES
        Utils
        BoolEdit
        ColorEdit
        ComboBox
        CursorEdit
        DateEdit
        DateTimeEdit
        DummyWidget
        FontEdit
        LineStyleEdit
        PixmapEdit
        PointEdit
        RectEdit
        SizeEdit
        SizePolicyEdit
        SpinBox
        StringEdit
        StringListEdit
        SymbolCombo
        TimeEdit
        UrlEdit
    REQUIRED_HEADERS kproperty_editors_HEADERS
)

install(
    FILES
        ${kproperty_editors_HEADERS}
    DESTINATION ${INCLUDE_INSTALL_DIR}/KProperty/kproperty/editors
    COMPONENT Devel
)

install(
    FILES
        ${kproperty_editors_FORWARDING_HEADERS}
    DESTINATION ${INCLUDE_INSTALL_DIR}/KProperty/KProperty
    COMPONENT Devel
)
