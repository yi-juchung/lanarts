/*
 * parse_drawable.h:
 *  Parses arbitrary drawable objects (eg, image based or lua defined animations)
 */

#ifndef PARSE_DRAWABLE_H_
#define PARSE_DRAWABLE_H_

#include <draw/LuaDrawable.h>
#include <draw/Image.h>
#include <draw/Animation.h>
#include <draw/DirectionalDrawable.h>
#include <draw/Image.h>
#include <draw/Drawable.h> // Generic wrapper
#include "../data/FilenameList.h"

namespace YAML {
class Node;
}

// Expects map, 'file:' indicates file to load
ldraw::Image parse_image(const YAML::Node& node);

// Expects map, 'frames:' indicate Drawable frames
ldraw::Animation parse_animation(const YAML::Node& node);

// Expects map, 'function:' is parsed as lua
ldraw::LuaDrawable parse_luadrawable(const YAML::Node& node);

ldraw::DirectionalDrawable parse_directional(const YAML::Node& node);

// If node is a string, loads multiple images based on a pattern
// If node is a list, calls parse_drawable on individual entries
std::vector<ldraw::Drawable> parse_drawable_list(const YAML::Node& node);

// If node is a string, loads multiple images based on a pattern
// If node is a list, calls parse_image on individual entries
std::vector<ldraw::Image> parse_image_list(const YAML::Node& node);

// Expects map, 'type:' indicates drawable type
// By default, 'type:' is 'image'
ldraw::Drawable parse_drawable(const YAML::Node& node);

#endif /* PARSE_DRAWABLE_H_ */