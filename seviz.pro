TEMPLATE = subdirs

SUBDIRS = \
        seviz_core \
        EngineTest

seviz_core.subdir = core
EngineTest.subdir = plugins/EngineTest
EngineTest.depends = seviz_core
