TEMPLATE = subdirs

SUBDIRS = \
        seviz_core \
        EngineTest \
        SentenceTree

seviz_core.subdir = core
EngineTest.subdir = plugins/EngineTest
EngineTest.depends = seviz_core
SentenceTree.subdir = plugins/SentenceTree
SentenceTree.depends = seviz_core
