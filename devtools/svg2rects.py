#!/usr/bin/env python3

import xml.etree.ElementTree as ET
import sys
import re


RECT_TAG_NAME = '{http://www.w3.org/2000/svg}rect'
TEXT_TAG_NAME = '{http://www.w3.org/2000/svg}text'


def color_from_style(style):
    m = re.match(".*fill:#([0-9a-z]{6}).*", style)
    return m.group(1)


def svg_rects(svg_root):
    return [rect for rect in svg_root.iter(RECT_TAG_NAME)]


def svg_texts(svg_root):
    return [rect for rect in svg_root.iter(TEXT_TAG_NAME)]


def save_background(svg_root, output_file):
    [background] = [rect
                    for rect in svg_rects(svg_root)
                    if rect.attrib['id'] == 'background']
    color = color_from_style(background.attrib['style'])
    output_file.write("%s\n" % (color))


def save_player(svg_root, output_file):
    [player] = [rect
                for rect in svg_rects(svg_root)
                if rect.attrib['id'] == 'player']
    color = color_from_style(player.attrib['style'])
    x = player.attrib['x']
    y = player.attrib['y']
    output_file.write("%s %s %s\n" % (x, y, color))


def save_platforms(svg_root, output_file):
    platforms = [rect
                 for rect in svg_rects(svg_root)
                 if rect.attrib['id'].startswith("rect")]

    output_file.write("%d\n" % (len(platforms)))
    for platform in platforms:
        x = platform.attrib['x']
        y = platform.attrib['y']
        w = platform.attrib['width']
        h = platform.attrib['height']
        color = color_from_style(platform.attrib['style'])
        output_file.write("%s %s %s %s %s\n" % (x, y, w, h, color))


def save_goals(svg_root, output_file):
    goals = [rect
             for rect in svg_rects(svg_root)
             if rect.attrib['id'].startswith("goal")]

    output_file.write("%d\n" % (len(goals)))

    for goal in goals:
        goal_id = goal.attrib['id'][len('goal'):]
        [region] = [rect
                    for rect in svg_rects(svg_root)
                    if rect.attrib['id'] == 'region' + goal_id]
        record = (goal.attrib['id'], goal.attrib['x'], goal.attrib['y'],
                  region.attrib['x'], region.attrib['y'],
                  region.attrib['width'], region.attrib['height'],
                  color_from_style(goal.attrib['style']))
        output_file.write("%s %s %s %s %s %s %s %s\n" % record)


def save_lavas(svg_root, output_file):
    lavas = [rect
             for rect in svg_rects(svg_root)
             if rect.attrib['id'].startswith('lava')]

    output_file.write("%d\n" % (len(lavas)))

    for lava in lavas:
        x = lava.attrib['x']
        y = lava.attrib['y']
        w = lava.attrib['width']
        h = lava.attrib['height']
        color = color_from_style(lava.attrib['style'])
        output_file.write("%s %s %s %s %s\n" % (x, y, w, h, color))


def save_backplatforms(svg_root, output_file):
    platforms = [rect
                 for rect in svg_rects(svg_root)
                 if rect.attrib['id'].startswith("backrect")]

    output_file.write("%d\n" % (len(platforms)))
    for platform in platforms:
        x = platform.attrib['x']
        y = platform.attrib['y']
        w = platform.attrib['width']
        h = platform.attrib['height']
        color = color_from_style(platform.attrib['style'])
        output_file.write("%s %s %s %s %s\n" % (x, y, w, h, color))


def save_boxes(svg_root, output_file):
    boxes = [rect
             for rect in svg_rects(svg_root)
             if rect.attrib['id'].startswith("box")]

    output_file.write("%d\n" % (len(boxes)))
    for box in boxes:
        box_id = box.attrib['id']
        x = box.attrib['x']
        y = box.attrib['y']
        w = box.attrib['width']
        h = box.attrib['height']
        color = color_from_style(box.attrib['style'])
        output_file.write("%s %s %s %s %s %s\n" % (box_id, x, y, w, h, color))


def save_labels(svg_root, output_file):
    labels = [text
              for text in svg_texts(svg_root)
              if text.attrib['id'].startswith('label')]

    output_file.write("%d\n" % (len(labels)))
    for label in labels:
        x = label.attrib['x']
        y = label.attrib['y']
        color = color_from_style(label.attrib['style'])
        # TODO(#432): svg2rects doesn't handle newlines in labels
        text = ' '.join([tspan.text for tspan in label])
        output_file.write("%s %s %s\n" % (x, y, color))
        output_file.write("%s\n" % (text))


def save_scripts(svg_root, output_file):
    scripts = [rect
               for rect in svg_rects(svg_root)
               if rect.attrib['id'].startswith('script')]

    output_file.write("%d\n" % (len(scripts)))
    for script in scripts:
        x = script.attrib['x']
        y = script.attrib['y']
        w = script.attrib['width']
        h = script.attrib['height']
        output_file.write("%s %s %s %s\n" % (x, y, w, h))
        [title] = [child for child in script]
        with open(title.text, 'r') as script_file:
            script_lines = script_file.read().splitlines()
            output_file.write("%d\n" % (len(script_lines)))
            for script_line in script_lines:
                output_file.write("%s\n" % script_line)

def svg2rects(svg_file_name, output_file_name):
    svg_tree = ET.parse(svg_file_name)
    svg_root = svg_tree.getroot()

    with open(output_file_name, "w") as output_file:
        save_background(svg_root, output_file)
        save_player(svg_root, output_file)
        save_platforms(svg_root, output_file)
        save_goals(svg_root, output_file)
        save_lavas(svg_root, output_file)
        save_backplatforms(svg_root, output_file)
        save_boxes(svg_root, output_file)
        save_labels(svg_root, output_file)
        save_scripts(svg_root, output_file)

def usage():
    print("Usage: svg2rects.py <svg-file-name> <output-file-name>")


if __name__ == '__main__':
    if len(sys.argv) < 3:
        usage()
        exit(1)

    svg2rects(sys.argv[1], sys.argv[2])
