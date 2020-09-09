Debug:DESTDIR = debug
Debug:OBJECTS_DIR = debug_tmp/obj
Debug:MOC_DIR = debug_tmp/moc
Debug:RCC_DIR = debug_tmp/rcc
Debug:UI_DIR = debug_tmp/ui

Release:DESTDIR = release
Release:OBJECTS_DIR = release_tmp/obj
Release:MOC_DIR = release_tmp/moc
Release:RCC_DIR = release_tmp/rcc
Release:UI_DIR = release_tmp/ui

defineTest(copyToDir) {
    files = $$1
    dir = $$2
    # replace slashes in destination path for Windows
    win32:dir ~= s,/,\\,g

    for(file, files) {
        # replace slashes in source path for Windows
        win32:file ~= s,/,\\,g

        QMAKE_POST_LINK += $$QMAKE_COPY_DIR $$shell_quote($$file) $$shell_quote($$dir) $$escape_expand(\\n\\t)
    }

    export(QMAKE_POST_LINK)
}
