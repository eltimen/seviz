import os
import sqlite3
from sqlite3 import Error

import xml.etree.ElementTree as ET

'''
Script generates relational sqlite db from FrameNet's xml files.
Must be placed to the framenet_v17 folder.

TODO May be apply database normalization here?
'''


def frameIndex():
    tree = ET.parse('frameIndex.xml')
    root = tree.getroot()
    for child in root:
        yield child.attrib
        
def luIndex():
    tree = ET.parse('luIndex.xml')
    root = tree.getroot()
    for child in root:
        if child.tag.endswith('lu'):
            yield child.attrib
         
def frame_elements(name):
    tree = ET.parse('frame\{}.xml'.format(name))
    root = tree.getroot()
    for child in root:
        if child.tag.endswith('FE'):
            yield child.attrib

def create_and_fill_db(path):
    conn = sqlite3.connect(path)
    c = conn.cursor()
    c.execute("""
                CREATE TABLE frames (
                    id integer PRIMARY_KEY,
                    name TEXT NOT NULL
                );
              """)
              
    c.execute("""
                CREATE TABLE lexical_units (
                    id integer PRIMARY_KEY,
                    frameName TEXT NOT NULL,
                    frameId integer NOT NULL,
                    text TEXT NOT NULL,
                    pos TEXT NOT NULL
                );
              """)
              
    c.execute("""
            CREATE TABLE frame_elements (
                id integer PRIMARY_KEY,
                name TEXT NOT NULL,
                bgColor TEXT NOT NULL,
                fgColor TEXT NOT NULL,
                frameId integer NOT NULL,
                frameName TEXT NOT NULL
            );
          """)
    
    for frame in frameIndex():
        c.execute("""
                INSERT INTO frames
                VALUES ( {}, '{}' );
              """.format(frame['ID'], frame['name']))
              
        for fe in frame_elements(frame['name']):
            c.execute("""
                INSERT INTO frame_elements
                VALUES ( ?, ?, ?, ?, ?, ?);
              """, (fe['ID'], fe['name'], fe['bgColor'], fe['fgColor'], frame['ID'], frame['name']))
    
    for lu in luIndex():
        lu_dot_index = lu['name'].rfind('.')
        lu_text = lu['name'][:lu_dot_index]
        lu_pos = lu['name'][lu_dot_index+1:]
        c.execute("""
                INSERT INTO lexical_units
                VALUES ( ?, ?, ?, ?, ?);
              """, (lu['ID'], lu['frameName'], lu['frameID'], lu_text, lu_pos))
              
    conn.commit()
    conn.close()

if __name__ == '__main__':
    try: 
        os.remove('framenet.db')
    except FileNotFoundError:
        pass
    create_and_fill_db('framenet.db')
