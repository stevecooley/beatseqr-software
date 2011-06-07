{
	"patcher" : 	{
		"fileversion" : 1,
		"rect" : [ 509.0, 116.0, 892.0, 352.0 ],
		"bglocked" : 0,
		"defrect" : [ 509.0, 116.0, 892.0, 352.0 ],
		"openrect" : [ 0.0, 0.0, 0.0, 0.0 ],
		"openinpresentation" : 0,
		"default_fontsize" : 12.0,
		"default_fontface" : 0,
		"default_fontname" : "Arial",
		"gridonopen" : 0,
		"gridsize" : [ 15.0, 15.0 ],
		"gridsnaponopen" : 0,
		"toolbarvisible" : 1,
		"boxanimatetime" : 200,
		"imprint" : 0,
		"enablehscroll" : 1,
		"enablevscroll" : 1,
		"devicewidth" : 0.0,
		"boxes" : [ 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "r actualswing",
					"outlettype" : [ "" ],
					"patching_rect" : [ 474.0, 232.0, 81.0, 20.0 ],
					"fontsize" : 12.0,
					"id" : "obj-74",
					"fontname" : "Arial",
					"numinlets" : 0,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "message",
					"text" : "100",
					"outlettype" : [ "" ],
					"patching_rect" : [ 422.0, 266.0, 32.5, 18.0 ],
					"fontsize" : 12.0,
					"id" : "obj-68",
					"fontname" : "Arial",
					"numinlets" : 2,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "t b i",
					"outlettype" : [ "bang", "int" ],
					"patching_rect" : [ 447.0, 292.0, 32.5, 20.0 ],
					"fontsize" : 12.0,
					"id" : "obj-64",
					"fontname" : "Arial",
					"numinlets" : 1,
					"numoutlets" : 2
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "number",
					"outlettype" : [ "int", "bang" ],
					"patching_rect" : [ 400.0, 326.0, 50.0, 20.0 ],
					"fontsize" : 12.0,
					"id" : "obj-56",
					"fontname" : "Arial",
					"numinlets" : 1,
					"numoutlets" : 2
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "expr $i1-$i2",
					"outlettype" : [ "" ],
					"patching_rect" : [ 455.0, 313.0, 73.0, 20.0 ],
					"fontsize" : 12.0,
					"id" : "obj-23",
					"fontname" : "Arial",
					"numinlets" : 2,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "r initvel",
					"outlettype" : [ "" ],
					"patching_rect" : [ 338.0, 226.0, 49.0, 20.0 ],
					"fontsize" : 12.0,
					"id" : "obj-9",
					"fontname" : "Arial",
					"numinlets" : 0,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "makenote 127 100",
					"outlettype" : [ "float", "float" ],
					"patching_rect" : [ 293.0, 256.0, 110.0, 20.0 ],
					"fontsize" : 12.0,
					"id" : "obj-10",
					"fontname" : "Arial",
					"numinlets" : 3,
					"numoutlets" : 2
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "message",
					"text" : "36",
					"outlettype" : [ "" ],
					"patching_rect" : [ 293.0, 226.0, 32.5, 18.0 ],
					"fontsize" : 12.0,
					"id" : "obj-11",
					"fontname" : "Arial",
					"numinlets" : 2,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "r all_notes_off",
					"outlettype" : [ "" ],
					"patching_rect" : [ 30.0, 60.0, 86.0, 20.0 ],
					"fontsize" : 12.0,
					"id" : "obj-7",
					"fontname" : "Arial",
					"numinlets" : 0,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "inlet",
					"outlettype" : [ "" ],
					"patching_rect" : [ 240.0, 225.0, 25.0, 25.0 ],
					"id" : "obj-79",
					"numinlets" : 0,
					"numoutlets" : 1,
					"comment" : "velocity"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "inlet",
					"outlettype" : [ "" ],
					"patching_rect" : [ 30.0, 150.0, 25.0, 25.0 ],
					"id" : "obj-78",
					"numinlets" : 0,
					"numoutlets" : 1,
					"comment" : "midi note num"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "inlet",
					"outlettype" : [ "" ],
					"patching_rect" : [ 165.0, 15.0, 25.0, 25.0 ],
					"id" : "obj-77",
					"numinlets" : 0,
					"numoutlets" : 1,
					"comment" : "incoming OSC data"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "receive steps",
					"outlettype" : [ "" ],
					"patching_rect" : [ 600.0, 30.0, 81.0, 20.0 ],
					"fontsize" : 12.0,
					"id" : "obj-2",
					"fontname" : "Arial",
					"numinlets" : 0,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "sel 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15",
					"outlettype" : [ "bang", "bang", "bang", "bang", "bang", "bang", "bang", "bang", "bang", "bang", "bang", "bang", "bang", "bang", "bang", "bang", "" ],
					"patching_rect" : [ 600.0, 60.0, 235.0, 20.0 ],
					"fontsize" : 12.0,
					"id" : "obj-1",
					"fontname" : "Arial",
					"numinlets" : 1,
					"numoutlets" : 17
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "outlet",
					"patching_rect" : [ 225.0, 285.0, 25.0, 25.0 ],
					"id" : "obj-59",
					"numinlets" : 1,
					"numoutlets" : 0,
					"comment" : "midi note num"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "number",
					"outlettype" : [ "int", "bang" ],
					"patching_rect" : [ 30.0, 195.0, 50.0, 20.0 ],
					"fontsize" : 12.0,
					"id" : "obj-57",
					"fontname" : "Arial",
					"numinlets" : 1,
					"numoutlets" : 2
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "r initvel",
					"outlettype" : [ "" ],
					"patching_rect" : [ 180.0, 225.0, 49.0, 20.0 ],
					"fontsize" : 12.0,
					"id" : "obj-6",
					"fontname" : "Arial",
					"numinlets" : 0,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "outlet",
					"patching_rect" : [ 180.0, 285.0, 25.0, 25.0 ],
					"id" : "obj-35",
					"numinlets" : 1,
					"numoutlets" : 0,
					"comment" : "velocity"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "outlet",
					"patching_rect" : [ 135.0, 285.0, 25.0, 25.0 ],
					"id" : "obj-36",
					"numinlets" : 1,
					"numoutlets" : 0,
					"comment" : "pitch"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "makenote 127 100",
					"outlettype" : [ "float", "float" ],
					"patching_rect" : [ 135.0, 255.0, 110.0, 20.0 ],
					"fontsize" : 12.0,
					"id" : "obj-67",
					"fontname" : "Arial",
					"numinlets" : 3,
					"numoutlets" : 2
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "message",
					"text" : "36",
					"outlettype" : [ "" ],
					"patching_rect" : [ 135.0, 225.0, 32.5, 18.0 ],
					"fontsize" : 12.0,
					"id" : "obj-5",
					"fontname" : "Arial",
					"numinlets" : 2,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "gate",
					"outlettype" : [ "" ],
					"patching_rect" : [ 810.0, 150.0, 34.0, 20.0 ],
					"fontsize" : 12.0,
					"id" : "obj-37",
					"fontname" : "Arial",
					"numinlets" : 2,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "button",
					"outlettype" : [ "bang" ],
					"patching_rect" : [ 810.0, 180.0, 20.0, 20.0 ],
					"id" : "obj-38",
					"numinlets" : 1,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "gate",
					"outlettype" : [ "" ],
					"patching_rect" : [ 765.0, 150.0, 34.0, 20.0 ],
					"fontsize" : 12.0,
					"id" : "obj-39",
					"fontname" : "Arial",
					"numinlets" : 2,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "button",
					"outlettype" : [ "bang" ],
					"patching_rect" : [ 765.0, 180.0, 20.0, 20.0 ],
					"id" : "obj-40",
					"numinlets" : 1,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "gate",
					"outlettype" : [ "" ],
					"patching_rect" : [ 720.0, 150.0, 34.0, 20.0 ],
					"fontsize" : 12.0,
					"id" : "obj-41",
					"fontname" : "Arial",
					"numinlets" : 2,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "button",
					"outlettype" : [ "bang" ],
					"patching_rect" : [ 720.0, 180.0, 20.0, 20.0 ],
					"id" : "obj-42",
					"numinlets" : 1,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "gate",
					"outlettype" : [ "" ],
					"patching_rect" : [ 675.0, 150.0, 34.0, 20.0 ],
					"fontsize" : 12.0,
					"id" : "obj-43",
					"fontname" : "Arial",
					"numinlets" : 2,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "button",
					"outlettype" : [ "bang" ],
					"patching_rect" : [ 675.0, 180.0, 20.0, 20.0 ],
					"id" : "obj-44",
					"numinlets" : 1,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "gate",
					"outlettype" : [ "" ],
					"patching_rect" : [ 630.0, 150.0, 34.0, 20.0 ],
					"fontsize" : 12.0,
					"id" : "obj-45",
					"fontname" : "Arial",
					"numinlets" : 2,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "button",
					"outlettype" : [ "bang" ],
					"patching_rect" : [ 630.0, 180.0, 20.0, 20.0 ],
					"id" : "obj-46",
					"numinlets" : 1,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "gate",
					"outlettype" : [ "" ],
					"patching_rect" : [ 585.0, 150.0, 34.0, 20.0 ],
					"fontsize" : 12.0,
					"id" : "obj-47",
					"fontname" : "Arial",
					"numinlets" : 2,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "button",
					"outlettype" : [ "bang" ],
					"patching_rect" : [ 585.0, 180.0, 20.0, 20.0 ],
					"id" : "obj-48",
					"numinlets" : 1,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "gate",
					"outlettype" : [ "" ],
					"patching_rect" : [ 540.0, 150.0, 34.0, 20.0 ],
					"fontsize" : 12.0,
					"id" : "obj-49",
					"fontname" : "Arial",
					"numinlets" : 2,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "button",
					"outlettype" : [ "bang" ],
					"patching_rect" : [ 540.0, 180.0, 20.0, 20.0 ],
					"id" : "obj-51",
					"numinlets" : 1,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "gate",
					"outlettype" : [ "" ],
					"patching_rect" : [ 495.0, 150.0, 34.0, 20.0 ],
					"fontsize" : 12.0,
					"id" : "obj-52",
					"fontname" : "Arial",
					"numinlets" : 2,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "button",
					"outlettype" : [ "bang" ],
					"patching_rect" : [ 495.0, 180.0, 20.0, 20.0 ],
					"id" : "obj-54",
					"numinlets" : 1,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "gate",
					"outlettype" : [ "" ],
					"patching_rect" : [ 450.0, 150.0, 34.0, 20.0 ],
					"fontsize" : 12.0,
					"id" : "obj-50",
					"fontname" : "Arial",
					"numinlets" : 2,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "button",
					"outlettype" : [ "bang" ],
					"patching_rect" : [ 450.0, 180.0, 20.0, 20.0 ],
					"id" : "obj-21",
					"numinlets" : 1,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "gate",
					"outlettype" : [ "" ],
					"patching_rect" : [ 405.0, 150.0, 34.0, 20.0 ],
					"fontsize" : 12.0,
					"id" : "obj-22",
					"fontname" : "Arial",
					"numinlets" : 2,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "button",
					"outlettype" : [ "bang" ],
					"patching_rect" : [ 405.0, 180.0, 20.0, 20.0 ],
					"id" : "obj-24",
					"numinlets" : 1,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "gate",
					"outlettype" : [ "" ],
					"patching_rect" : [ 360.0, 150.0, 34.0, 20.0 ],
					"fontsize" : 12.0,
					"id" : "obj-53",
					"fontname" : "Arial",
					"numinlets" : 2,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "button",
					"outlettype" : [ "bang" ],
					"patching_rect" : [ 360.0, 180.0, 20.0, 20.0 ],
					"id" : "obj-55",
					"numinlets" : 1,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "gate",
					"outlettype" : [ "" ],
					"patching_rect" : [ 315.0, 150.0, 34.0, 20.0 ],
					"fontsize" : 12.0,
					"id" : "obj-58",
					"fontname" : "Arial",
					"numinlets" : 2,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "button",
					"outlettype" : [ "bang" ],
					"patching_rect" : [ 315.0, 180.0, 20.0, 20.0 ],
					"id" : "obj-60",
					"numinlets" : 1,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "gate",
					"outlettype" : [ "" ],
					"patching_rect" : [ 270.0, 150.0, 34.0, 20.0 ],
					"fontsize" : 12.0,
					"id" : "obj-63",
					"fontname" : "Arial",
					"numinlets" : 2,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "button",
					"outlettype" : [ "bang" ],
					"patching_rect" : [ 270.0, 180.0, 20.0, 20.0 ],
					"id" : "obj-65",
					"numinlets" : 1,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "gate",
					"outlettype" : [ "" ],
					"patching_rect" : [ 225.0, 150.0, 34.0, 20.0 ],
					"fontsize" : 12.0,
					"id" : "obj-66",
					"fontname" : "Arial",
					"numinlets" : 2,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "button",
					"outlettype" : [ "bang" ],
					"patching_rect" : [ 225.0, 180.0, 20.0, 20.0 ],
					"id" : "obj-69",
					"numinlets" : 1,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "gate",
					"outlettype" : [ "" ],
					"patching_rect" : [ 180.0, 150.0, 34.0, 20.0 ],
					"fontsize" : 12.0,
					"id" : "obj-71",
					"fontname" : "Arial",
					"numinlets" : 2,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "button",
					"outlettype" : [ "bang" ],
					"patching_rect" : [ 180.0, 180.0, 20.0, 20.0 ],
					"id" : "obj-72",
					"numinlets" : 1,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "gate",
					"outlettype" : [ "" ],
					"patching_rect" : [ 135.0, 150.0, 34.0, 20.0 ],
					"fontsize" : 12.0,
					"id" : "obj-73",
					"fontname" : "Arial",
					"numinlets" : 2,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "button",
					"outlettype" : [ "bang" ],
					"patching_rect" : [ 135.0, 180.0, 20.0, 20.0 ],
					"id" : "obj-3",
					"numinlets" : 1,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "toggle",
					"outlettype" : [ "int" ],
					"patching_rect" : [ 810.0, 105.0, 20.0, 20.0 ],
					"id" : "obj-25",
					"numinlets" : 1,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "toggle",
					"outlettype" : [ "int" ],
					"patching_rect" : [ 765.0, 105.0, 20.0, 20.0 ],
					"id" : "obj-26",
					"numinlets" : 1,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "toggle",
					"outlettype" : [ "int" ],
					"patching_rect" : [ 720.0, 105.0, 20.0, 20.0 ],
					"id" : "obj-27",
					"numinlets" : 1,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "toggle",
					"outlettype" : [ "int" ],
					"patching_rect" : [ 675.0, 105.0, 20.0, 20.0 ],
					"id" : "obj-28",
					"numinlets" : 1,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "toggle",
					"outlettype" : [ "int" ],
					"patching_rect" : [ 630.0, 105.0, 20.0, 20.0 ],
					"id" : "obj-29",
					"numinlets" : 1,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "toggle",
					"outlettype" : [ "int" ],
					"patching_rect" : [ 585.0, 105.0, 20.0, 20.0 ],
					"id" : "obj-30",
					"numinlets" : 1,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "toggle",
					"outlettype" : [ "int" ],
					"patching_rect" : [ 540.0, 105.0, 20.0, 20.0 ],
					"id" : "obj-31",
					"numinlets" : 1,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "toggle",
					"outlettype" : [ "int" ],
					"patching_rect" : [ 495.0, 105.0, 20.0, 20.0 ],
					"id" : "obj-32",
					"numinlets" : 1,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "toggle",
					"outlettype" : [ "int" ],
					"patching_rect" : [ 450.0, 105.0, 20.0, 20.0 ],
					"id" : "obj-17",
					"numinlets" : 1,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "toggle",
					"outlettype" : [ "int" ],
					"patching_rect" : [ 405.0, 105.0, 20.0, 20.0 ],
					"id" : "obj-18",
					"numinlets" : 1,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "toggle",
					"outlettype" : [ "int" ],
					"patching_rect" : [ 360.0, 105.0, 20.0, 20.0 ],
					"id" : "obj-19",
					"numinlets" : 1,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "toggle",
					"outlettype" : [ "int" ],
					"patching_rect" : [ 315.0, 105.0, 20.0, 20.0 ],
					"id" : "obj-20",
					"numinlets" : 1,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "toggle",
					"outlettype" : [ "int" ],
					"patching_rect" : [ 270.0, 105.0, 20.0, 20.0 ],
					"id" : "obj-15",
					"numinlets" : 1,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "toggle",
					"outlettype" : [ "int" ],
					"patching_rect" : [ 225.0, 105.0, 20.0, 20.0 ],
					"id" : "obj-16",
					"numinlets" : 1,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "toggle",
					"outlettype" : [ "int" ],
					"patching_rect" : [ 180.0, 105.0, 20.0, 20.0 ],
					"id" : "obj-14",
					"numinlets" : 1,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "toggle",
					"outlettype" : [ "int" ],
					"patching_rect" : [ 135.0, 105.0, 20.0, 20.0 ],
					"id" : "obj-4",
					"numinlets" : 1,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "OSC-route /0 /1 /2 /3 /4 /5 /6 /7 /8 /9 /10 /11 /12 /13 /14 /15",
					"outlettype" : [ "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "" ],
					"patching_rect" : [ 165.0, 60.0, 320.0, 20.0 ],
					"fontsize" : 12.0,
					"id" : "obj-70",
					"fontname" : "Arial",
					"numinlets" : 1,
					"numoutlets" : 17
				}

			}
 ],
		"lines" : [ 			{
				"patchline" : 				{
					"source" : [ "obj-23", 0 ],
					"destination" : [ "obj-10", 2 ],
					"hidden" : 0,
					"midpoints" : [ 464.5, 333.0, 450.0, 333.0, 450.0, 312.0, 405.0, 312.0, 405.0, 252.0, 393.5, 252.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-9", 0 ],
					"destination" : [ "obj-10", 1 ],
					"hidden" : 0,
					"midpoints" : [ 347.5, 252.0, 348.0, 252.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-11", 0 ],
					"destination" : [ "obj-10", 0 ],
					"hidden" : 0,
					"midpoints" : [ 302.5, 246.0, 302.5, 246.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-79", 0 ],
					"destination" : [ "obj-10", 1 ],
					"hidden" : 0,
					"midpoints" : [ 249.5, 252.0, 348.0, 252.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-10", 0 ],
					"destination" : [ "obj-36", 0 ],
					"hidden" : 0,
					"midpoints" : [ 302.5, 276.0, 261.0, 276.0, 261.0, 282.0, 144.5, 282.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-10", 1 ],
					"destination" : [ "obj-35", 0 ],
					"hidden" : 0,
					"midpoints" : [ 393.5, 288.0, 261.0, 288.0, 261.0, 282.0, 189.5, 282.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-79", 0 ],
					"destination" : [ "obj-67", 1 ],
					"hidden" : 0,
					"midpoints" : [ 249.5, 252.0, 190.0, 252.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-67", 1 ],
					"destination" : [ "obj-35", 0 ],
					"hidden" : 0,
					"midpoints" : [ 235.5, 276.0, 192.0, 276.0, 192.0, 282.0, 189.5, 282.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-67", 0 ],
					"destination" : [ "obj-36", 0 ],
					"hidden" : 0,
					"midpoints" : [ 144.5, 276.0, 144.5, 276.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-5", 0 ],
					"destination" : [ "obj-67", 0 ],
					"hidden" : 0,
					"midpoints" : [ 144.5, 243.0, 144.5, 243.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-6", 0 ],
					"destination" : [ "obj-67", 1 ],
					"hidden" : 0,
					"midpoints" : [ 189.5, 246.0, 190.0, 246.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-7", 0 ],
					"destination" : [ "obj-25", 0 ],
					"hidden" : 0,
					"midpoints" : [ 39.5, 90.0, 819.5, 90.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-7", 0 ],
					"destination" : [ "obj-26", 0 ],
					"hidden" : 0,
					"midpoints" : [ 39.5, 90.0, 774.5, 90.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-7", 0 ],
					"destination" : [ "obj-27", 0 ],
					"hidden" : 0,
					"midpoints" : [ 39.5, 90.0, 729.5, 90.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-7", 0 ],
					"destination" : [ "obj-28", 0 ],
					"hidden" : 0,
					"midpoints" : [ 39.5, 90.0, 684.5, 90.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-7", 0 ],
					"destination" : [ "obj-29", 0 ],
					"hidden" : 0,
					"midpoints" : [ 39.5, 90.0, 639.5, 90.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-7", 0 ],
					"destination" : [ "obj-30", 0 ],
					"hidden" : 0,
					"midpoints" : [ 39.5, 90.0, 594.5, 90.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-7", 0 ],
					"destination" : [ "obj-31", 0 ],
					"hidden" : 0,
					"midpoints" : [ 39.5, 90.0, 549.5, 90.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-7", 0 ],
					"destination" : [ "obj-32", 0 ],
					"hidden" : 0,
					"midpoints" : [ 39.5, 90.0, 504.5, 90.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-7", 0 ],
					"destination" : [ "obj-17", 0 ],
					"hidden" : 0,
					"midpoints" : [ 39.5, 90.0, 459.5, 90.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-7", 0 ],
					"destination" : [ "obj-18", 0 ],
					"hidden" : 0,
					"midpoints" : [ 39.5, 90.0, 414.5, 90.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-7", 0 ],
					"destination" : [ "obj-19", 0 ],
					"hidden" : 0,
					"midpoints" : [ 39.5, 90.0, 369.5, 90.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-7", 0 ],
					"destination" : [ "obj-20", 0 ],
					"hidden" : 0,
					"midpoints" : [ 39.5, 90.0, 324.5, 90.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-7", 0 ],
					"destination" : [ "obj-15", 0 ],
					"hidden" : 0,
					"midpoints" : [ 39.5, 90.0, 279.5, 90.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-7", 0 ],
					"destination" : [ "obj-16", 0 ],
					"hidden" : 0,
					"midpoints" : [ 39.5, 90.0, 234.5, 90.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-7", 0 ],
					"destination" : [ "obj-14", 0 ],
					"hidden" : 0,
					"midpoints" : [ 39.5, 90.0, 189.5, 90.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-7", 0 ],
					"destination" : [ "obj-4", 0 ],
					"hidden" : 0,
					"midpoints" : [ 39.5, 90.0, 144.5, 90.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-78", 0 ],
					"destination" : [ "obj-57", 0 ],
					"hidden" : 0,
					"midpoints" : [ 39.5, 177.0, 39.5, 177.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-77", 0 ],
					"destination" : [ "obj-70", 0 ],
					"hidden" : 0,
					"midpoints" : [ 174.5, 42.0, 174.5, 42.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-1", 15 ],
					"destination" : [ "obj-37", 1 ],
					"hidden" : 0,
					"midpoints" : [ 812.0, 90.0, 840.0, 90.0, 840.0, 135.0, 834.5, 135.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-1", 14 ],
					"destination" : [ "obj-39", 1 ],
					"hidden" : 0,
					"midpoints" : [ 798.5, 90.0, 795.0, 90.0, 795.0, 135.0, 789.5, 135.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-1", 13 ],
					"destination" : [ "obj-41", 1 ],
					"hidden" : 0,
					"midpoints" : [ 785.0, 90.0, 750.0, 90.0, 750.0, 135.0, 744.5, 135.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-1", 12 ],
					"destination" : [ "obj-43", 1 ],
					"hidden" : 0,
					"midpoints" : [ 771.5, 90.0, 705.0, 90.0, 705.0, 135.0, 699.5, 135.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-1", 11 ],
					"destination" : [ "obj-45", 1 ],
					"hidden" : 0,
					"midpoints" : [ 758.0, 90.0, 660.0, 90.0, 660.0, 135.0, 654.5, 135.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-1", 10 ],
					"destination" : [ "obj-47", 1 ],
					"hidden" : 0,
					"midpoints" : [ 744.5, 90.0, 615.0, 90.0, 615.0, 135.0, 609.5, 135.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-1", 9 ],
					"destination" : [ "obj-49", 1 ],
					"hidden" : 0,
					"midpoints" : [ 731.0, 90.0, 570.0, 90.0, 570.0, 135.0, 564.5, 135.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-1", 8 ],
					"destination" : [ "obj-52", 1 ],
					"hidden" : 0,
					"midpoints" : [ 717.5, 90.0, 525.0, 90.0, 525.0, 135.0, 519.5, 135.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-1", 7 ],
					"destination" : [ "obj-50", 1 ],
					"hidden" : 0,
					"midpoints" : [ 704.0, 90.0, 480.0, 90.0, 480.0, 135.0, 474.5, 135.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-1", 6 ],
					"destination" : [ "obj-22", 1 ],
					"hidden" : 0,
					"midpoints" : [ 690.5, 90.0, 435.0, 90.0, 435.0, 135.0, 429.5, 135.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-1", 5 ],
					"destination" : [ "obj-53", 1 ],
					"hidden" : 0,
					"midpoints" : [ 677.0, 90.0, 390.0, 90.0, 390.0, 135.0, 384.5, 135.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-1", 4 ],
					"destination" : [ "obj-58", 1 ],
					"hidden" : 0,
					"midpoints" : [ 663.5, 90.0, 345.0, 90.0, 345.0, 135.0, 339.5, 135.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-1", 3 ],
					"destination" : [ "obj-63", 1 ],
					"hidden" : 0,
					"midpoints" : [ 650.0, 90.0, 300.0, 90.0, 300.0, 135.0, 294.5, 135.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-1", 2 ],
					"destination" : [ "obj-66", 1 ],
					"hidden" : 0,
					"midpoints" : [ 636.5, 90.0, 255.0, 90.0, 255.0, 135.0, 249.5, 135.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-1", 1 ],
					"destination" : [ "obj-71", 1 ],
					"hidden" : 0,
					"midpoints" : [ 623.0, 90.0, 210.0, 90.0, 210.0, 135.0, 204.5, 135.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-1", 0 ],
					"destination" : [ "obj-73", 1 ],
					"hidden" : 0,
					"midpoints" : [ 609.5, 90.0, 165.0, 90.0, 165.0, 135.0, 159.5, 135.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-2", 0 ],
					"destination" : [ "obj-1", 0 ],
					"hidden" : 0,
					"midpoints" : [ 609.5, 51.0, 609.5, 51.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-25", 0 ],
					"destination" : [ "obj-37", 0 ],
					"hidden" : 0,
					"midpoints" : [ 819.5, 126.0, 819.5, 126.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-26", 0 ],
					"destination" : [ "obj-39", 0 ],
					"hidden" : 0,
					"midpoints" : [ 774.5, 126.0, 774.5, 126.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-27", 0 ],
					"destination" : [ "obj-41", 0 ],
					"hidden" : 0,
					"midpoints" : [ 729.5, 126.0, 729.5, 126.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-28", 0 ],
					"destination" : [ "obj-43", 0 ],
					"hidden" : 0,
					"midpoints" : [ 684.5, 126.0, 684.5, 126.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-29", 0 ],
					"destination" : [ "obj-45", 0 ],
					"hidden" : 0,
					"midpoints" : [ 639.5, 126.0, 639.5, 126.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-30", 0 ],
					"destination" : [ "obj-47", 0 ],
					"hidden" : 0,
					"midpoints" : [ 594.5, 126.0, 594.5, 126.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-31", 0 ],
					"destination" : [ "obj-49", 0 ],
					"hidden" : 0,
					"midpoints" : [ 549.5, 126.0, 549.5, 126.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-32", 0 ],
					"destination" : [ "obj-52", 0 ],
					"hidden" : 0,
					"midpoints" : [ 504.5, 126.0, 504.5, 126.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-17", 0 ],
					"destination" : [ "obj-50", 0 ],
					"hidden" : 0,
					"midpoints" : [ 459.5, 126.0, 459.5, 126.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-18", 0 ],
					"destination" : [ "obj-22", 0 ],
					"hidden" : 0,
					"midpoints" : [ 414.5, 126.0, 414.5, 126.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-19", 0 ],
					"destination" : [ "obj-53", 0 ],
					"hidden" : 0,
					"midpoints" : [ 369.5, 126.0, 369.5, 126.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-20", 0 ],
					"destination" : [ "obj-58", 0 ],
					"hidden" : 0,
					"midpoints" : [ 324.5, 126.0, 324.5, 126.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-15", 0 ],
					"destination" : [ "obj-63", 0 ],
					"hidden" : 0,
					"midpoints" : [ 279.5, 126.0, 279.5, 126.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-16", 0 ],
					"destination" : [ "obj-66", 0 ],
					"hidden" : 0,
					"midpoints" : [ 234.5, 126.0, 234.5, 126.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-14", 0 ],
					"destination" : [ "obj-71", 0 ],
					"hidden" : 0,
					"midpoints" : [ 189.5, 126.0, 189.5, 126.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-4", 0 ],
					"destination" : [ "obj-73", 0 ],
					"hidden" : 0,
					"midpoints" : [ 144.5, 126.0, 144.5, 126.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-5", 0 ],
					"destination" : [ "obj-59", 0 ],
					"hidden" : 0,
					"midpoints" : [ 144.5, 252.0, 132.0, 252.0, 132.0, 279.0, 234.5, 279.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-40", 0 ],
					"destination" : [ "obj-5", 0 ],
					"hidden" : 0,
					"midpoints" : [ 774.5, 210.0, 144.5, 210.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-44", 0 ],
					"destination" : [ "obj-5", 0 ],
					"hidden" : 0,
					"midpoints" : [ 684.5, 210.0, 144.5, 210.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-48", 0 ],
					"destination" : [ "obj-5", 0 ],
					"hidden" : 0,
					"midpoints" : [ 594.5, 210.0, 144.5, 210.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-54", 0 ],
					"destination" : [ "obj-5", 0 ],
					"hidden" : 0,
					"midpoints" : [ 504.5, 210.0, 144.5, 210.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-24", 0 ],
					"destination" : [ "obj-5", 0 ],
					"hidden" : 0,
					"midpoints" : [ 414.5, 210.0, 144.5, 210.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-60", 0 ],
					"destination" : [ "obj-5", 0 ],
					"hidden" : 0,
					"midpoints" : [ 324.5, 210.0, 144.5, 210.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-69", 0 ],
					"destination" : [ "obj-5", 0 ],
					"hidden" : 0,
					"midpoints" : [ 234.5, 210.0, 144.5, 210.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-3", 0 ],
					"destination" : [ "obj-5", 0 ],
					"hidden" : 0,
					"midpoints" : [ 144.5, 201.0, 144.5, 201.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-52", 0 ],
					"destination" : [ "obj-54", 0 ],
					"hidden" : 0,
					"midpoints" : [ 504.5, 171.0, 504.5, 171.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-49", 0 ],
					"destination" : [ "obj-51", 0 ],
					"hidden" : 0,
					"midpoints" : [ 549.5, 171.0, 549.5, 171.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-45", 0 ],
					"destination" : [ "obj-46", 0 ],
					"hidden" : 0,
					"midpoints" : [ 639.5, 171.0, 639.5, 171.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-47", 0 ],
					"destination" : [ "obj-48", 0 ],
					"hidden" : 0,
					"midpoints" : [ 594.5, 171.0, 594.5, 171.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-39", 0 ],
					"destination" : [ "obj-40", 0 ],
					"hidden" : 0,
					"midpoints" : [ 774.5, 171.0, 774.5, 171.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-37", 0 ],
					"destination" : [ "obj-38", 0 ],
					"hidden" : 0,
					"midpoints" : [ 819.5, 171.0, 819.5, 171.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-41", 0 ],
					"destination" : [ "obj-42", 0 ],
					"hidden" : 0,
					"midpoints" : [ 729.5, 171.0, 729.5, 171.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-43", 0 ],
					"destination" : [ "obj-44", 0 ],
					"hidden" : 0,
					"midpoints" : [ 684.5, 171.0, 684.5, 171.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-58", 0 ],
					"destination" : [ "obj-60", 0 ],
					"hidden" : 0,
					"midpoints" : [ 324.5, 171.0, 324.5, 171.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-53", 0 ],
					"destination" : [ "obj-55", 0 ],
					"hidden" : 0,
					"midpoints" : [ 369.5, 171.0, 369.5, 171.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-50", 0 ],
					"destination" : [ "obj-21", 0 ],
					"hidden" : 0,
					"midpoints" : [ 459.5, 171.0, 459.5, 171.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-22", 0 ],
					"destination" : [ "obj-24", 0 ],
					"hidden" : 0,
					"midpoints" : [ 414.5, 171.0, 414.5, 171.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-66", 0 ],
					"destination" : [ "obj-69", 0 ],
					"hidden" : 0,
					"midpoints" : [ 234.5, 171.0, 234.5, 171.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-63", 0 ],
					"destination" : [ "obj-65", 0 ],
					"hidden" : 0,
					"midpoints" : [ 279.5, 171.0, 279.5, 171.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-71", 0 ],
					"destination" : [ "obj-72", 0 ],
					"hidden" : 0,
					"midpoints" : [ 189.5, 171.0, 189.5, 171.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-73", 0 ],
					"destination" : [ "obj-3", 0 ],
					"hidden" : 0,
					"midpoints" : [ 144.5, 171.0, 144.5, 171.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-57", 0 ],
					"destination" : [ "obj-5", 1 ],
					"hidden" : 0,
					"midpoints" : [ 39.5, 225.0, 120.0, 225.0, 120.0, 210.0, 158.0, 210.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-70", 15 ],
					"destination" : [ "obj-25", 0 ],
					"hidden" : 0,
					"midpoints" : [ 456.6875, 90.0, 819.5, 90.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-70", 14 ],
					"destination" : [ "obj-26", 0 ],
					"hidden" : 0,
					"midpoints" : [ 437.875, 90.0, 774.5, 90.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-70", 13 ],
					"destination" : [ "obj-27", 0 ],
					"hidden" : 0,
					"midpoints" : [ 419.0625, 90.0, 729.5, 90.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-70", 12 ],
					"destination" : [ "obj-28", 0 ],
					"hidden" : 0,
					"midpoints" : [ 400.25, 90.0, 684.5, 90.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-70", 11 ],
					"destination" : [ "obj-29", 0 ],
					"hidden" : 0,
					"midpoints" : [ 381.4375, 90.0, 639.5, 90.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-70", 10 ],
					"destination" : [ "obj-30", 0 ],
					"hidden" : 0,
					"midpoints" : [ 362.625, 90.0, 594.5, 90.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-70", 9 ],
					"destination" : [ "obj-31", 0 ],
					"hidden" : 0,
					"midpoints" : [ 343.8125, 90.0, 549.5, 90.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-70", 8 ],
					"destination" : [ "obj-32", 0 ],
					"hidden" : 0,
					"midpoints" : [ 325.0, 90.0, 504.5, 90.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-70", 7 ],
					"destination" : [ "obj-17", 0 ],
					"hidden" : 0,
					"midpoints" : [ 306.1875, 90.0, 459.5, 90.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-70", 6 ],
					"destination" : [ "obj-18", 0 ],
					"hidden" : 0,
					"midpoints" : [ 287.375, 90.0, 414.5, 90.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-70", 5 ],
					"destination" : [ "obj-19", 0 ],
					"hidden" : 0,
					"midpoints" : [ 268.5625, 90.0, 369.5, 90.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-70", 4 ],
					"destination" : [ "obj-20", 0 ],
					"hidden" : 0,
					"midpoints" : [ 249.75, 90.0, 324.5, 90.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-70", 3 ],
					"destination" : [ "obj-15", 0 ],
					"hidden" : 0,
					"midpoints" : [ 230.9375, 90.0, 279.5, 90.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-70", 2 ],
					"destination" : [ "obj-16", 0 ],
					"hidden" : 0,
					"midpoints" : [ 212.125, 90.0, 234.5, 90.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-70", 1 ],
					"destination" : [ "obj-14", 0 ],
					"hidden" : 0,
					"midpoints" : [ 193.3125, 102.0, 189.5, 102.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-70", 0 ],
					"destination" : [ "obj-4", 0 ],
					"hidden" : 0,
					"midpoints" : [ 174.5, 90.0, 144.5, 90.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-11", 0 ],
					"destination" : [ "obj-59", 0 ],
					"hidden" : 0,
					"midpoints" : [ 302.5, 246.0, 276.0, 246.0, 276.0, 282.0, 234.5, 282.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-68", 0 ],
					"destination" : [ "obj-64", 0 ],
					"hidden" : 0,
					"midpoints" : [ 431.5, 285.0, 456.5, 285.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-74", 0 ],
					"destination" : [ "obj-23", 1 ],
					"hidden" : 0,
					"midpoints" : [ 483.5, 279.0, 518.5, 279.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-64", 1 ],
					"destination" : [ "obj-23", 0 ],
					"hidden" : 0,
					"midpoints" : [ 470.0, 312.0, 464.5, 312.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-64", 0 ],
					"destination" : [ "obj-23", 0 ],
					"hidden" : 0,
					"midpoints" : [ 456.5, 312.0, 464.5, 312.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-23", 0 ],
					"destination" : [ "obj-56", 0 ],
					"hidden" : 0,
					"midpoints" : [ 464.5, 333.0, 450.0, 333.0, 450.0, 321.0, 409.5, 321.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-74", 0 ],
					"destination" : [ "obj-68", 0 ],
					"hidden" : 0,
					"midpoints" : [ 483.5, 252.0, 431.5, 252.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-72", 0 ],
					"destination" : [ "obj-11", 0 ],
					"hidden" : 0,
					"midpoints" : [ 189.5, 210.0, 302.5, 210.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-57", 0 ],
					"destination" : [ "obj-11", 1 ],
					"hidden" : 0,
					"midpoints" : [ 39.5, 225.0, 120.0, 225.0, 120.0, 210.0, 316.0, 210.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-65", 0 ],
					"destination" : [ "obj-11", 0 ],
					"hidden" : 0,
					"midpoints" : [ 279.5, 213.0, 302.5, 213.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-55", 0 ],
					"destination" : [ "obj-11", 0 ],
					"hidden" : 0,
					"midpoints" : [ 369.5, 213.0, 302.5, 213.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-21", 0 ],
					"destination" : [ "obj-11", 0 ],
					"hidden" : 0,
					"midpoints" : [ 459.5, 213.0, 302.5, 213.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-51", 0 ],
					"destination" : [ "obj-11", 0 ],
					"hidden" : 0,
					"midpoints" : [ 549.5, 213.0, 302.5, 213.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-46", 0 ],
					"destination" : [ "obj-11", 0 ],
					"hidden" : 0,
					"midpoints" : [ 639.5, 213.0, 302.5, 213.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-42", 0 ],
					"destination" : [ "obj-11", 0 ],
					"hidden" : 0,
					"midpoints" : [ 729.5, 213.0, 302.5, 213.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-38", 0 ],
					"destination" : [ "obj-11", 0 ],
					"hidden" : 0,
					"midpoints" : [ 819.5, 213.0, 302.5, 213.0 ]
				}

			}
 ]
	}

}
