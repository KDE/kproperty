set(KPROPERTY_EDITORS_HEADERS
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
)

ecm_generate_headers(kproperty_editors_FORWARDING_HEADERS
    REQUIRED_HEADERS kproperty_editors_HEADERS
    # TODO: rename to camelcase, then uncomment "ORIGINAL CAMELCASE"
    HEADER_NAMES ${KPROPERTY_EDITORS_HEADERS}
)

ecm_generate_headers(kproperty_editors_FORWARDING_HEADERS
    REQUIRED_HEADERS kproperty_editors_HEADERS
    ORIGINAL CAMELCASE
    HEADER_NAMES
        KPropertyGenericSelectionEditor
        KPropertyMultiLineStringEditor
        KPropertyRectFEditor
        KPropertyStringEditor
        KPropertyUrlEditor
)

set(kproperty_editors_HEADERS ${kproperty_editors_HEADERS} PARENT_SCOPE)
set(kproperty_editors_FORWARDING_HEADERS ${kproperty_editors_FORWARDING_HEADERS} PARENT_SCOPE)
