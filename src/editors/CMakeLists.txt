ecm_generate_headers(kproperty_editors_FORWARDING_HEADERS
    REQUIRED_HEADERS kproperty_editors_HEADERS
    # TODO: rename to camelcase, then uncomment "ORIGINAL CAMELCASE"
    PREFIX kproperty
    HEADER_NAMES
        Utils
        BoolEdit
        ColorEdit
        ComboBox
        CursorEdit
        DateEdit
        DateTimeEdit
        #DummyWidget
        FontEdit
        LineStyleEdit
        PixmapEdit
        PointEdit
        RectEdit
        SizeEdit
        SizePolicyEdit
        SpinBox
        #StringListEdit
        #SymbolCombo
        TimeEdit
        #TODO UrlEdit
)

ecm_generate_headers(kproperty_editors_FORWARDING_HEADERS
    REQUIRED_HEADERS kproperty_editors_HEADERS
    ORIGINAL CAMELCASE
    PREFIX kproperty
    HEADER_NAMES
        KPropertyStringEditor
)

install(
    FILES
        ${kproperty_editors_HEADERS}
    DESTINATION ${INCLUDE_INSTALL_DIR}/KProperty/kproperty
    COMPONENT Devel
)

install(
    FILES
        ${kproperty_editors_FORWARDING_HEADERS}
    DESTINATION ${INCLUDE_INSTALL_DIR}/KProperty
    COMPONENT Devel
)
