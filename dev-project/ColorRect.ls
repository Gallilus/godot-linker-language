[script_settings]

tool=false
valid=false
abstract=false
reloading=false
link_count=12

[script_values]

members=Array[StringName]([&"_ready", &"sadasdas", &"ds"])
methods={
"_ready": {
"args": Array[Dictionary]([]),
"default_args": [],
"flags": 8,
"id": 0,
"name": "_ready",
"return": {
"class_name": &"",
"hint": 0,
"hint_string": "",
"name": "",
"type": 0,
"usage": 6
}
},
"sadasdas": {
"args": Array[Dictionary]([]),
"default_args": [],
"flags": 1,
"id": 0,
"name": "sadasdas",
"return": {
"class_name": &"",
"hint": 0,
"hint_string": "",
"name": "",
"type": 0,
"usage": 6
}
}
}
properties={
"ds": {
"default_value": Color(1, 0, 0, 0),
"getter": &"",
"index": 0,
"info": {
"class_name": &"",
"hint": 0,
"hint_string": "",
"name": "ds",
"type": 20,
"usage": 6
},
"is_constant": false,
"is_export": false,
"is_private": false,
"setter": &""
}
}
constants={}
signals={}
links=Array[LinkerLink]([Object(LinkerFunction,"resource_local_to_scene":false,"resource_name":"","links_idx":0,"index_name":&"_ready","pull_links_idx ":[],"push_links_idx":[],"owner_idx":-1,"script":null)
, Object(LinkerIndexGet,"resource_local_to_scene":false,"resource_name":"","links_idx":1,"index_name":&"texture","pull_links_idx ":[7],"push_links_idx":[],"owner_idx":-1,"script":null)
, Object(LinkerIndexGet,"resource_local_to_scene":false,"resource_name":"","links_idx":2,"index_name":&"color","pull_links_idx ":[7],"push_links_idx":[],"owner_idx":-1,"script":null)
, Object(LinkerIndexCall,"resource_local_to_scene":false,"resource_name":"","links_idx":3,"index_name":&"print","pull_links_idx ":[5],"push_links_idx":[],"owner_idx":0,"script":null)
, Object(LinkerSceneRefrence,"resource_local_to_scene":false,"resource_name":"","links_idx":4,"index_name":&"../../GridMap","pull_links_idx ":[],"push_links_idx":[],"owner_idx":-1,"node_class_name":&"GridMap","node_name":&"GridMap","node_script_file_path":"","node_scene_path":NodePath("GridMap"),"node_scene_relative_path":&"../../GridMap","script":null)
, Object(LinkerIndexGet,"resource_local_to_scene":false,"resource_name":"","links_idx":5,"index_name":&"cell_size","pull_links_idx ":[4],"push_links_idx":[],"owner_idx":-1,"script":null)
, Object(LinkerIndexCall,"resource_local_to_scene":false,"resource_name":"","links_idx":6,"index_name":&"print","pull_links_idx ":[11],"push_links_idx":[],"owner_idx":2,"script":null)
, Object(LinkerSceneRefrence,"resource_local_to_scene":false,"resource_name":"","links_idx":7,"index_name":&".","pull_links_idx ":[],"push_links_idx":[],"owner_idx":-1,"node_class_name":&"ColorRect","node_name":&"ColorRect","node_script_file_path":"res://ColorRect.ls","node_scene_path":NodePath("Control/ColorRect"),"node_scene_relative_path":&".","script":null)
, Object(LinkerSceneRefrence,"resource_local_to_scene":false,"resource_name":"","links_idx":8,"index_name":&"TextureRect","pull_links_idx ":[],"push_links_idx":[],"owner_idx":-1,"node_class_name":&"TextureRect","node_name":&"TextureRect","node_script_file_path":"res://TextureRect.gd","node_scene_path":NodePath("Control/ColorRect/TextureRect"),"node_scene_relative_path":&"TextureRect","script":null)
, Object(LinkerIndexCall,"resource_local_to_scene":false,"resource_name":"","links_idx":9,"index_name":&"set_color","pull_links_idx ":[3],"push_links_idx":[],"owner_idx":-1,"script":null)
, Object(LinkerIndexGet,"resource_local_to_scene":false,"resource_name":"","links_idx":10,"index_name":&"color","pull_links_idx ":[11],"push_links_idx":[],"owner_idx":-1,"script":null)
, Object(LinkerIndexGet,"resource_local_to_scene":false,"resource_name":"","links_idx":11,"index_name":&"custom_minimum_size","pull_links_idx ":[2],"push_links_idx":[],"owner_idx":-1,"script":null)
])
