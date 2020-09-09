TEMPLATE = subdirs

SUBDIRS = \
        seviz \
        EngineTest

EngineTest.subdir = plugins_src/EngineTest
EngineTest.depends = seviz
