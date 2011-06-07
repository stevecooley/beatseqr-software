{
	"patcher" : 	{
		"fileversion" : 1,
		"rect" : [ 109.0, 44.0, 887.0, 789.0 ],
		"bgcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
		"bglocked" : 0,
		"defrect" : [ 109.0, 44.0, 887.0, 789.0 ],
		"openrect" : [ 0.0, 0.0, 0.0, 0.0 ],
		"openinpresentation" : 0,
		"default_fontsize" : 10.0,
		"default_fontface" : 0,
		"default_fontname" : "Arial",
		"gridonopen" : 0,
		"gridsize" : [ 15.0, 15.0 ],
		"gridsnaponopen" : 0,
		"toolbarvisible" : 1,
		"boxanimatetime" : 200,
		"imprint" : 1,
		"enablehscroll" : 1,
		"enablevscroll" : 1,
		"devicewidth" : 0.0,
		"boxes" : [ 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "p stepdataformatter",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontface" : 0,
					"fontsize" : 10.0,
					"background" : 0,
					"color" : [ 0.8, 0.84, 0.71, 1.0 ],
					"numinlets" : 2,
					"ignoreclick" : 0,
					"numoutlets" : 1,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"fontname" : "Arial",
					"outlettype" : [ "" ],
					"patching_rect" : [ 615.0, 705.0, 97.0, 18.0 ],
					"presentation" : 0,
					"id" : "obj-21",
					"patcher" : 					{
						"fileversion" : 1,
						"rect" : [ 50.0, 94.0, 640.0, 480.0 ],
						"bglocked" : 0,
						"defrect" : [ 50.0, 94.0, 640.0, 480.0 ],
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
						"visible" : 1,
						"boxes" : [ 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "pack i i",
									"fontsize" : 12.0,
									"numinlets" : 2,
									"numoutlets" : 1,
									"fontname" : "Arial",
									"outlettype" : [ "" ],
									"patching_rect" : [ 180.0, 195.0, 48.0, 20.0 ],
									"id" : "obj-5"
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "message",
									"text" : "set $1+/+$2",
									"fontsize" : 12.0,
									"numinlets" : 2,
									"numoutlets" : 1,
									"fontname" : "Arial",
									"outlettype" : [ "" ],
									"patching_rect" : [ 180.0, 255.0, 74.0, 18.0 ],
									"id" : "obj-4"
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "outlet",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 180.0, 330.0, 25.0, 25.0 ],
									"id" : "obj-3",
									"comment" : ""
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "inlet",
									"numinlets" : 0,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 240.0, 150.0, 25.0, 25.0 ],
									"id" : "obj-2",
									"comment" : ""
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "inlet",
									"numinlets" : 0,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 180.0, 150.0, 25.0, 25.0 ],
									"id" : "obj-1",
									"comment" : ""
								}

							}
 ],
						"lines" : [ 							{
								"patchline" : 								{
									"source" : [ "obj-5", 0 ],
									"destination" : [ "obj-4", 0 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-2", 0 ],
									"destination" : [ "obj-5", 1 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-1", 0 ],
									"destination" : [ "obj-5", 0 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-4", 0 ],
									"destination" : [ "obj-3", 0 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
 ]
					}
,
					"saved_object_attributes" : 					{
						"fontface" : 0,
						"fontsize" : 12.0,
						"globalpatchername" : "",
						"default_fontface" : 0,
						"default_fontname" : "Arial",
						"fontname" : "Arial",
						"default_fontsize" : 12.0
					}

				}

			}
, 			{
				"box" : 				{
					"maxclass" : "message",
					"text" : "set $1",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontface" : 0,
					"fontsize" : 12.0,
					"background" : 0,
					"bgcolor2" : [ 0.867, 0.867, 0.867, 1.0 ],
					"numinlets" : 2,
					"ignoreclick" : 0,
					"gradient" : 0,
					"numoutlets" : 1,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 0.867, 0.867, 0.867, 1.0 ],
					"fontname" : "Arial",
					"outlettype" : [ "" ],
					"patching_rect" : [ 495.0, 660.0, 43.0, 18.0 ],
					"presentation" : 0,
					"id" : "obj-13"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "button",
					"outlinecolor" : [ 0.71328, 0.71328, 0.71328, 1.0 ],
					"background" : 0,
					"blinkcolor" : [ 1.0, 0.890196, 0.090196, 1.0 ],
					"numinlets" : 1,
					"ignoreclick" : 0,
					"numoutlets" : 1,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 0.91328, 0.91328, 0.91328, 0.75 ],
					"outlettype" : [ "bang" ],
					"fgcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"patching_rect" : [ 88.0, 756.0, 67.0, 67.0 ],
					"presentation" : 0,
					"id" : "obj-124"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "t b b b b b",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontface" : 0,
					"fontsize" : 10.0,
					"background" : 0,
					"color" : [ 0.8, 0.84, 0.71, 1.0 ],
					"numinlets" : 1,
					"ignoreclick" : 0,
					"numoutlets" : 5,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"fontname" : "Arial",
					"outlettype" : [ "bang", "bang", "bang", "bang", "bang" ],
					"patching_rect" : [ 554.0, 551.0, 73.0, 18.0 ],
					"presentation" : 0,
					"id" : "obj-122"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "pack s i",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontface" : 0,
					"fontsize" : 10.0,
					"background" : 0,
					"color" : [ 0.8, 0.84, 0.71, 1.0 ],
					"numinlets" : 2,
					"ignoreclick" : 0,
					"numoutlets" : 1,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"fontname" : "Arial",
					"outlettype" : [ "" ],
					"patching_rect" : [ 595.0, 799.0, 44.0, 18.0 ],
					"presentation" : 0,
					"id" : "obj-121"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "s oscbang",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontface" : 0,
					"fontsize" : 10.0,
					"background" : 0,
					"color" : [ 0.8, 0.84, 0.71, 1.0 ],
					"numinlets" : 1,
					"ignoreclick" : 0,
					"numoutlets" : 0,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"fontname" : "Arial",
					"patching_rect" : [ 90.0, 840.0, 56.0, 18.0 ],
					"presentation" : 0,
					"id" : "obj-120"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "r oscbang",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontface" : 0,
					"fontsize" : 10.0,
					"background" : 0,
					"color" : [ 0.8, 0.84, 0.71, 1.0 ],
					"numinlets" : 0,
					"ignoreclick" : 0,
					"numoutlets" : 1,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"fontname" : "Arial",
					"outlettype" : [ "" ],
					"patching_rect" : [ 390.0, 793.0, 54.0, 18.0 ],
					"presentation" : 0,
					"id" : "obj-119"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "message",
					"text" : "set $1",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontface" : 0,
					"fontsize" : 12.0,
					"background" : 0,
					"bgcolor2" : [ 0.867, 0.867, 0.867, 1.0 ],
					"numinlets" : 2,
					"ignoreclick" : 0,
					"gradient" : 0,
					"numoutlets" : 1,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 0.867, 0.867, 0.867, 1.0 ],
					"fontname" : "Arial",
					"outlettype" : [ "" ],
					"patching_rect" : [ 486.0, 750.0, 43.0, 18.0 ],
					"presentation" : 0,
					"id" : "obj-118"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "pack i i",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontface" : 0,
					"fontsize" : 10.0,
					"background" : 0,
					"color" : [ 0.8, 0.84, 0.71, 1.0 ],
					"numinlets" : 2,
					"ignoreclick" : 0,
					"numoutlets" : 1,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"fontname" : "Arial",
					"outlettype" : [ "" ],
					"patching_rect" : [ 510.0, 630.0, 42.0, 18.0 ],
					"presentation" : 0,
					"id" : "obj-117"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "message",
					"text" : "/beatseqr/matrix/0/0",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontface" : 0,
					"fontsize" : 12.0,
					"background" : 0,
					"bgcolor2" : [ 0.867, 0.867, 0.867, 1.0 ],
					"numinlets" : 2,
					"ignoreclick" : 0,
					"gradient" : 0,
					"numoutlets" : 1,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 0.867, 0.867, 0.867, 1.0 ],
					"fontname" : "Arial",
					"outlettype" : [ "" ],
					"patching_rect" : [ 485.0, 775.0, 124.0, 18.0 ],
					"presentation" : 0,
					"id" : "obj-115"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "s oscmess",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontface" : 0,
					"fontsize" : 10.0,
					"background" : 0,
					"color" : [ 0.8, 0.84, 0.71, 1.0 ],
					"numinlets" : 1,
					"ignoreclick" : 0,
					"numoutlets" : 0,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"fontname" : "Arial",
					"patching_rect" : [ 528.0, 827.0, 56.0, 18.0 ],
					"presentation" : 0,
					"id" : "obj-110"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "print",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontface" : 0,
					"fontsize" : 10.0,
					"background" : 0,
					"color" : [ 0.8, 0.84, 0.71, 1.0 ],
					"numinlets" : 1,
					"ignoreclick" : 0,
					"numoutlets" : 0,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"fontname" : "Arial",
					"patching_rect" : [ 360.0, 870.0, 30.0, 18.0 ],
					"presentation" : 0,
					"id" : "obj-107"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "js",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontface" : 0,
					"fontsize" : 10.0,
					"background" : 0,
					"color" : [ 0.8, 0.84, 0.71, 1.0 ],
					"numinlets" : 1,
					"ignoreclick" : 0,
					"numoutlets" : 1,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"fontname" : "Arial",
					"outlettype" : [ "" ],
					"patching_rect" : [ 484.0, 723.0, 19.0, 18.0 ],
					"presentation" : 0,
					"id" : "obj-95"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "message",
					"text" : "/beatseqr/patternselect $1",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontface" : 0,
					"fontsize" : 12.0,
					"background" : 0,
					"bgcolor2" : [ 0.867, 0.867, 0.867, 1.0 ],
					"numinlets" : 2,
					"ignoreclick" : 0,
					"gradient" : 0,
					"numoutlets" : 1,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 0.867, 0.867, 0.867, 1.0 ],
					"fontname" : "Arial",
					"outlettype" : [ "" ],
					"patching_rect" : [ 277.0, 680.0, 149.0, 18.0 ],
					"presentation" : 0,
					"id" : "obj-91"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "message",
					"text" : "/beatseqr/swing $1",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontface" : 0,
					"fontsize" : 12.0,
					"background" : 0,
					"bgcolor2" : [ 0.867, 0.867, 0.867, 1.0 ],
					"numinlets" : 2,
					"ignoreclick" : 0,
					"gradient" : 0,
					"numoutlets" : 1,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 0.867, 0.867, 0.867, 1.0 ],
					"fontname" : "Arial",
					"outlettype" : [ "" ],
					"patching_rect" : [ 255.0, 709.0, 111.0, 18.0 ],
					"presentation" : 0,
					"id" : "obj-89"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "message",
					"text" : "/beatseqr/octaveadjust $1",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontface" : 0,
					"fontsize" : 12.0,
					"background" : 0,
					"bgcolor2" : [ 0.867, 0.867, 0.867, 1.0 ],
					"numinlets" : 2,
					"ignoreclick" : 0,
					"gradient" : 0,
					"numoutlets" : 1,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 0.867, 0.867, 0.867, 1.0 ],
					"fontname" : "Arial",
					"outlettype" : [ "" ],
					"patching_rect" : [ 240.0, 735.0, 148.0, 18.0 ],
					"presentation" : 0,
					"id" : "obj-87"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "message",
					"text" : "/beatseqr/tempo $1",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontface" : 0,
					"fontsize" : 12.0,
					"background" : 0,
					"bgcolor2" : [ 0.867, 0.867, 0.867, 1.0 ],
					"numinlets" : 2,
					"ignoreclick" : 0,
					"gradient" : 0,
					"numoutlets" : 1,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 0.867, 0.867, 0.867, 1.0 ],
					"fontname" : "Arial",
					"outlettype" : [ "" ],
					"patching_rect" : [ 210.0, 756.0, 114.0, 18.0 ],
					"presentation" : 0,
					"id" : "obj-79"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "message",
					"text" : "/beatseqr/tempoadjust $1",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontface" : 0,
					"fontsize" : 12.0,
					"background" : 0,
					"bgcolor2" : [ 0.867, 0.867, 0.867, 1.0 ],
					"numinlets" : 2,
					"ignoreclick" : 0,
					"gradient" : 0,
					"numoutlets" : 1,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 0.867, 0.867, 0.867, 1.0 ],
					"fontname" : "Arial",
					"outlettype" : [ "" ],
					"patching_rect" : [ 183.0, 786.0, 146.0, 18.0 ],
					"presentation" : 0,
					"id" : "obj-82"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "s oscmess",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontface" : 0,
					"fontsize" : 10.0,
					"background" : 0,
					"color" : [ 0.8, 0.84, 0.71, 1.0 ],
					"numinlets" : 1,
					"ignoreclick" : 0,
					"numoutlets" : 0,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"fontname" : "Arial",
					"patching_rect" : [ 240.0, 855.0, 58.0, 18.0 ],
					"presentation" : 0,
					"id" : "obj-86"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "s oscmess",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontface" : 0,
					"fontsize" : 10.0,
					"background" : 0,
					"color" : [ 0.8, 0.84, 0.71, 1.0 ],
					"numinlets" : 1,
					"ignoreclick" : 0,
					"numoutlets" : 0,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"fontname" : "Arial",
					"patching_rect" : [ 210.0, 855.0, 58.0, 18.0 ],
					"presentation" : 0,
					"id" : "obj-85"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "r oscmess",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontface" : 0,
					"fontsize" : 10.0,
					"background" : 0,
					"color" : [ 0.8, 0.84, 0.71, 1.0 ],
					"numinlets" : 0,
					"ignoreclick" : 0,
					"numoutlets" : 1,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"fontname" : "Arial",
					"outlettype" : [ "" ],
					"patching_rect" : [ 390.0, 825.0, 56.0, 18.0 ],
					"presentation" : 0,
					"id" : "obj-84"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "message",
					"text" : "debug",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontface" : 0,
					"fontsize" : 9.0,
					"background" : 0,
					"bgcolor2" : [ 0.867, 0.867, 0.867, 1.0 ],
					"numinlets" : 2,
					"ignoreclick" : 0,
					"gradient" : 0,
					"numoutlets" : 1,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 0.867, 0.867, 0.867, 1.0 ],
					"fontname" : "Arial",
					"outlettype" : [ "" ],
					"patching_rect" : [ 315.0, 840.0, 40.0, 15.0 ],
					"presentation" : 0,
					"id" : "obj-80"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "udpsend 127.0.0.1 8111",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontface" : 0,
					"fontsize" : 9.0,
					"background" : 0,
					"color" : [ 0.8, 0.84, 0.71, 1.0 ],
					"numinlets" : 1,
					"ignoreclick" : 0,
					"numoutlets" : 0,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"fontname" : "Arial",
					"patching_rect" : [ 405.0, 870.0, 106.0, 17.0 ],
					"presentation" : 0,
					"id" : "obj-77"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "OpenSoundControl 2000",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontface" : 0,
					"fontsize" : 9.0,
					"background" : 0,
					"color" : [ 0.8, 0.84, 0.71, 1.0 ],
					"numinlets" : 1,
					"ignoreclick" : 0,
					"numoutlets" : 3,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"fontname" : "Arial",
					"outlettype" : [ "", "", "OSCTimeTag" ],
					"patching_rect" : [ 420.0, 900.0, 115.0, 17.0 ],
					"presentation" : 0,
					"id" : "obj-78"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "OpenSoundControl",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontface" : 0,
					"fontsize" : 10.0,
					"background" : 0,
					"color" : [ 0.8, 0.84, 0.71, 1.0 ],
					"numinlets" : 1,
					"ignoreclick" : 0,
					"numoutlets" : 3,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"fontname" : "Arial",
					"outlettype" : [ "", "", "OSCTimeTag" ],
					"patching_rect" : [ 645.0, 270.0, 96.0, 18.0 ],
					"presentation" : 0,
					"id" : "obj-76"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "message",
					"text" : "store 4",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontface" : 0,
					"fontsize" : 11.595187,
					"background" : 0,
					"bgcolor2" : [ 0.867, 0.867, 0.867, 1.0 ],
					"numinlets" : 2,
					"ignoreclick" : 0,
					"gradient" : 0,
					"numoutlets" : 1,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 0.867, 0.867, 0.867, 1.0 ],
					"fontname" : "Arial",
					"outlettype" : [ "" ],
					"patching_rect" : [ 555.0, 240.0, 46.0, 18.0 ],
					"presentation" : 0,
					"id" : "obj-75"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "message",
					"text" : "store 3",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontface" : 0,
					"fontsize" : 11.595187,
					"background" : 0,
					"bgcolor2" : [ 0.867, 0.867, 0.867, 1.0 ],
					"numinlets" : 2,
					"ignoreclick" : 0,
					"gradient" : 0,
					"numoutlets" : 1,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 0.867, 0.867, 0.867, 1.0 ],
					"fontname" : "Arial",
					"outlettype" : [ "" ],
					"patching_rect" : [ 510.0, 240.0, 46.0, 18.0 ],
					"presentation" : 0,
					"id" : "obj-74"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "+ 1",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontface" : 0,
					"fontsize" : 10.0,
					"background" : 0,
					"color" : [ 0.8, 0.84, 0.71, 1.0 ],
					"numinlets" : 2,
					"ignoreclick" : 0,
					"numoutlets" : 1,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"fontname" : "Arial",
					"outlettype" : [ "int" ],
					"patching_rect" : [ 345.0, 375.0, 32.5, 18.0 ],
					"presentation" : 0,
					"id" : "obj-73"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "message",
					"text" : "2",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontface" : 0,
					"fontsize" : 11.595187,
					"background" : 0,
					"bgcolor2" : [ 0.867, 0.867, 0.867, 1.0 ],
					"numinlets" : 2,
					"ignoreclick" : 0,
					"gradient" : 0,
					"numoutlets" : 1,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 0.867, 0.867, 0.867, 1.0 ],
					"fontname" : "Arial",
					"outlettype" : [ "" ],
					"patching_rect" : [ 525.0, 315.0, 32.5, 18.0 ],
					"presentation" : 0,
					"id" : "obj-22"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "message",
					"text" : "1",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontface" : 0,
					"fontsize" : 11.595187,
					"background" : 0,
					"bgcolor2" : [ 0.867, 0.867, 0.867, 1.0 ],
					"numinlets" : 2,
					"ignoreclick" : 0,
					"gradient" : 0,
					"numoutlets" : 1,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 0.867, 0.867, 0.867, 1.0 ],
					"fontname" : "Arial",
					"outlettype" : [ "" ],
					"patching_rect" : [ 495.0, 315.0, 32.5, 18.0 ],
					"presentation" : 0,
					"id" : "obj-20"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "message",
					"text" : "store 2",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontface" : 0,
					"fontsize" : 11.595187,
					"background" : 0,
					"bgcolor2" : [ 0.867, 0.867, 0.867, 1.0 ],
					"numinlets" : 2,
					"ignoreclick" : 0,
					"gradient" : 0,
					"numoutlets" : 1,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 0.867, 0.867, 0.867, 1.0 ],
					"fontname" : "Arial",
					"outlettype" : [ "" ],
					"patching_rect" : [ 465.0, 240.0, 46.0, 18.0 ],
					"presentation" : 0,
					"id" : "obj-19"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "message",
					"text" : "store 1",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontface" : 0,
					"fontsize" : 11.595187,
					"background" : 0,
					"bgcolor2" : [ 0.867, 0.867, 0.867, 1.0 ],
					"numinlets" : 2,
					"ignoreclick" : 0,
					"gradient" : 0,
					"numoutlets" : 1,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 0.867, 0.867, 0.867, 1.0 ],
					"fontname" : "Arial",
					"outlettype" : [ "" ],
					"patching_rect" : [ 420.0, 240.0, 44.0, 18.0 ],
					"presentation" : 0,
					"id" : "obj-16"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "print matrix",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontface" : 0,
					"fontsize" : 11.595187,
					"background" : 0,
					"color" : [ 0.8, 0.84, 0.71, 1.0 ],
					"numinlets" : 1,
					"ignoreclick" : 0,
					"numoutlets" : 0,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"fontname" : "Arial",
					"patching_rect" : [ 750.0, 930.0, 68.0, 20.0 ],
					"presentation" : 0,
					"id" : "obj-15"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "message",
					"text" : "1",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontface" : 0,
					"fontsize" : 12.0,
					"background" : 0,
					"bgcolor2" : [ 0.867, 0.867, 0.867, 1.0 ],
					"numinlets" : 2,
					"ignoreclick" : 0,
					"gradient" : 0,
					"numoutlets" : 1,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 0.867, 0.867, 0.867, 1.0 ],
					"fontname" : "Arial",
					"outlettype" : [ "" ],
					"patching_rect" : [ 502.0, 574.0, 25.0, 18.0 ],
					"presentation" : 0,
					"id" : "obj-72"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "message",
					"text" : "1",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontface" : 0,
					"fontsize" : 12.0,
					"background" : 0,
					"bgcolor2" : [ 0.867, 0.867, 0.867, 1.0 ],
					"numinlets" : 2,
					"ignoreclick" : 0,
					"gradient" : 0,
					"numoutlets" : 1,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 0.867, 0.867, 0.867, 1.0 ],
					"fontname" : "Arial",
					"outlettype" : [ "" ],
					"patching_rect" : [ 495.0, 690.0, 57.0, 18.0 ],
					"presentation" : 0,
					"id" : "obj-71"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "pack i i i",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontface" : 0,
					"fontsize" : 10.0,
					"background" : 0,
					"color" : [ 0.8, 0.84, 0.71, 1.0 ],
					"numinlets" : 3,
					"ignoreclick" : 0,
					"numoutlets" : 1,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"fontname" : "Arial",
					"outlettype" : [ "" ],
					"patching_rect" : [ 634.0, 651.0, 47.0, 18.0 ],
					"presentation" : 0,
					"id" : "obj-50"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "matrixctrl",
					"verticalmargin" : 1,
					"background" : 0,
					"horizontalmargin" : 1,
					"one/matrix" : 0,
					"active" : 1,
					"scale" : 1,
					"numinlets" : 1,
					"clickedimage" : 1,
					"range" : 2,
					"cellpict" : "<default>",
					"imagemask" : 0,
					"ignoreclick" : 0,
					"columns" : 8,
					"numoutlets" : 2,
					"rows" : 16,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"one/row" : 0,
					"inactiveimage" : 1,
					"hidden" : 0,
					"invisiblebkgnd" : 0,
					"outlettype" : [ "list", "list" ],
					"dialmode" : 0,
					"autosize" : 0,
					"verticalspacing" : 0,
					"bkgndpict" : "<default>",
					"patching_rect" : [ 700.0, 430.0, 124.0, 258.0 ],
					"horizontalspacing" : 0,
					"presentation" : 0,
					"dialtracking" : 0,
					"id" : "obj-46",
					"one/column" : 0
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "preset",
					"circlecolor" : [ 0.2, 0.2, 0.2, 0.8 ],
					"textcolor" : [ 0.5, 0.5, 0.5, 1.0 ],
					"active2" : [ 0.55, 0.568, 0.82, 1.0 ],
					"fontface" : 1,
					"fontsize" : 11.0,
					"background" : 0,
					"spacing" : 3,
					"stored1" : [ 0.8, 0.8, 0.8, 1.0 ],
					"clicked2" : [ 0.576, 0.733, 0.514, 1.0 ],
					"numinlets" : 1,
					"ignoreclick" : 0,
					"showtrack" : 1,
					"active1" : [ 0.79, 0.83, 1.0, 1.0 ],
					"numoutlets" : 4,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 0.94, 0.94, 0.94, 1.0 ],
					"embed" : 1,
					"emptycolor" : [ 0.83, 0.83, 0.83, 1.0 ],
					"clicked1" : [ 0.68, 0.937, 0.658, 1.0 ],
					"fontname" : "Arial",
					"outlettype" : [ "preset", "int", "preset", "int" ],
					"bordercolor" : [ 0.5, 0.5, 0.5, 1.0 ],
					"stored2" : [ 0.41, 0.41, 0.41, 1.0 ],
					"patching_rect" : [ 427.0, 357.0, 135.0, 74.0 ],
					"margin" : 3,
					"bubblesize" : 10,
					"presentation" : 0,
					"id" : "obj-43",
					"preset_data" : [ 						{
							"number" : 1,
							"data" : [ 5, "obj-3", "number", "int", 9, 5, "obj-6", "toggle", "int", 1, 5, "obj-23", "number", "int", 10, 5, "obj-37", "toggle", "int", 0, 256, "obj-46", "matrixctrl", "list", 0, 0, 0, 0, 1, 0, 0, 2, 0, 0, 3, 0, 0, 4, 0, 0, 5, 0, 0, 6, 0, 0, 7, 0, 0, 8, 0, 0, 9, 0, 0, 10, 0, 0, 11, 0, 0, 12, 0, 0, 13, 0, 0, 14, 0, 0, 15, 0, 1, 0, 0, 1, 1, 0, 1, 2, 0, 1, 3, 0, 1, 4, 0, 1, 5, 0, 1, 6, 0, 1, 7, 0, 1, 8, 0, 1, 9, 0, 1, 10, 0, 1, 11, 0, 1, 12, 0, 1, 13, 0, 1, 14, 0, 1, 15, 0, 2, 0, 0, 2, 1, 0, 2, 2, 0, 2, 3, 0, 2, 4, 0, 2, 5, 0, 2, 6, 0, 2, 7, 0, 2, 8, 0, 2, 9, 0, 2, 10, 0, 2, 11, 0, 2, 12, 0, 2, 13, 0, 2, 14, 0, 2, 15, 0, 3, 0, 0, 3, 1, 0, 3, 2, 0, 3, 3, 0, 3, 4, 0, 3, 5, 0, 3, 6, 0, 3, 7, 0, 3, 8, 0, 3, 9, 0, 3, 10, 0, 3, 11, 0, 3, 12, 0, 3, 13, 0, 3, 14, 0, 3, 15, 0, 4, 0, 0, 4, 1, 0, 4, 2, 0, 4, 3, 0, 4, 4, 0, 4, 5, 0, 4, 6, 0, 4, 7, 0, 4, 8, 0, 4, 9, 0, 4, 10, 0, 4, 11, 0, 4, 12, 0, 4, 13, 0, 4, 14, 0, 4, 15, 0, 5, 0, 0, 5, 1, 0, 5, 2, 0, 5, 3, 0, 136, "obj-46", "matrixctrl", "list", 5, 4, 0, 5, 5, 0, 5, 6, 0, 5, 7, 0, 5, 8, 0, 5, 9, 0, 5, 10, 0, 5, 11, 0, 5, 12, 0, 5, 13, 0, 5, 14, 0, 5, 15, 0, 6, 0, 0, 6, 1, 0, 6, 2, 0, 6, 3, 0, 6, 4, 0, 6, 5, 0, 6, 6, 0, 6, 7, 0, 6, 8, 0, 6, 9, 0, 6, 10, 0, 6, 11, 0, 6, 12, 0, 6, 13, 0, 6, 14, 0, 6, 15, 0, 7, 0, 0, 7, 1, 0, 7, 2, 0, 7, 3, 0, 7, 4, 0, 7, 5, 0, 7, 6, 0, 7, 7, 0, 7, 8, 0, 7, 9, 0, 7, 10, 0, 7, 11, 0, 7, 12, 0, 7, 13, 0, 7, 14, 0, 7, 15, 0 ]
						}
, 						{
							"number" : 2,
							"data" : [ 5, "obj-3", "number", "int", 6, 5, "obj-6", "toggle", "int", 1, 5, "obj-23", "number", "int", 10, 5, "obj-37", "toggle", "int", 0, 256, "obj-46", "matrixctrl", "list", 0, 0, 0, 0, 1, 0, 0, 2, 0, 0, 3, 0, 0, 4, 0, 0, 5, 0, 0, 6, 0, 0, 7, 0, 0, 8, 0, 0, 9, 0, 0, 10, 0, 0, 11, 0, 0, 12, 0, 0, 13, 0, 0, 14, 0, 0, 15, 0, 1, 0, 0, 1, 1, 0, 1, 2, 0, 1, 3, 0, 1, 4, 0, 1, 5, 0, 1, 6, 0, 1, 7, 0, 1, 8, 0, 1, 9, 0, 1, 10, 0, 1, 11, 0, 1, 12, 0, 1, 13, 0, 1, 14, 0, 1, 15, 0, 2, 0, 0, 2, 1, 0, 2, 2, 0, 2, 3, 0, 2, 4, 0, 2, 5, 0, 2, 6, 0, 2, 7, 0, 2, 8, 0, 2, 9, 0, 2, 10, 0, 2, 11, 0, 2, 12, 0, 2, 13, 0, 2, 14, 0, 2, 15, 0, 3, 0, 0, 3, 1, 0, 3, 2, 0, 3, 3, 0, 3, 4, 0, 3, 5, 0, 3, 6, 0, 3, 7, 0, 3, 8, 0, 3, 9, 0, 3, 10, 0, 3, 11, 0, 3, 12, 0, 3, 13, 0, 3, 14, 0, 3, 15, 0, 4, 0, 0, 4, 1, 0, 4, 2, 0, 4, 3, 0, 4, 4, 0, 4, 5, 0, 4, 6, 0, 4, 7, 0, 4, 8, 0, 4, 9, 0, 4, 10, 0, 4, 11, 0, 4, 12, 0, 4, 13, 0, 4, 14, 0, 4, 15, 0, 5, 0, 0, 5, 1, 0, 5, 2, 0, 5, 3, 0, 136, "obj-46", "matrixctrl", "list", 5, 4, 0, 5, 5, 0, 5, 6, 0, 5, 7, 0, 5, 8, 0, 5, 9, 0, 5, 10, 0, 5, 11, 0, 5, 12, 0, 5, 13, 0, 5, 14, 0, 5, 15, 0, 6, 0, 0, 6, 1, 0, 6, 2, 0, 6, 3, 0, 6, 4, 0, 6, 5, 0, 6, 6, 0, 6, 7, 0, 6, 8, 0, 6, 9, 0, 6, 10, 0, 6, 11, 0, 6, 12, 0, 6, 13, 0, 6, 14, 0, 6, 15, 0, 7, 0, 0, 7, 1, 0, 7, 2, 0, 7, 3, 0, 7, 4, 0, 7, 5, 0, 7, 6, 0, 7, 7, 0, 7, 8, 0, 7, 9, 0, 7, 10, 0, 7, 11, 0, 7, 12, 0, 7, 13, 0, 7, 14, 0, 7, 15, 0 ]
						}
, 						{
							"number" : 3,
							"data" : [ 5, "obj-3", "number", "int", 2, 5, "obj-6", "toggle", "int", 1, 5, "obj-23", "number", "int", 10, 5, "obj-37", "toggle", "int", 0, 256, "obj-46", "matrixctrl", "list", 0, 0, 0, 0, 1, 0, 0, 2, 0, 0, 3, 0, 0, 4, 0, 0, 5, 0, 0, 6, 0, 0, 7, 0, 0, 8, 0, 0, 9, 0, 0, 10, 0, 0, 11, 0, 0, 12, 0, 0, 13, 0, 0, 14, 0, 0, 15, 0, 1, 0, 0, 1, 1, 0, 1, 2, 0, 1, 3, 0, 1, 4, 0, 1, 5, 0, 1, 6, 0, 1, 7, 0, 1, 8, 0, 1, 9, 0, 1, 10, 0, 1, 11, 0, 1, 12, 0, 1, 13, 0, 1, 14, 0, 1, 15, 0, 2, 0, 0, 2, 1, 0, 2, 2, 0, 2, 3, 0, 2, 4, 0, 2, 5, 0, 2, 6, 0, 2, 7, 0, 2, 8, 0, 2, 9, 0, 2, 10, 0, 2, 11, 0, 2, 12, 0, 2, 13, 0, 2, 14, 0, 2, 15, 0, 3, 0, 0, 3, 1, 0, 3, 2, 0, 3, 3, 0, 3, 4, 0, 3, 5, 0, 3, 6, 0, 3, 7, 0, 3, 8, 0, 3, 9, 0, 3, 10, 0, 3, 11, 0, 3, 12, 0, 3, 13, 0, 3, 14, 0, 3, 15, 0, 4, 0, 0, 4, 1, 0, 4, 2, 0, 4, 3, 0, 4, 4, 0, 4, 5, 0, 4, 6, 0, 4, 7, 0, 4, 8, 0, 4, 9, 0, 4, 10, 0, 4, 11, 0, 4, 12, 0, 4, 13, 0, 4, 14, 0, 4, 15, 0, 5, 0, 0, 5, 1, 0, 5, 2, 0, 5, 3, 0, 136, "obj-46", "matrixctrl", "list", 5, 4, 0, 5, 5, 0, 5, 6, 0, 5, 7, 0, 5, 8, 0, 5, 9, 0, 5, 10, 0, 5, 11, 0, 5, 12, 0, 5, 13, 0, 5, 14, 0, 5, 15, 0, 6, 0, 0, 6, 1, 0, 6, 2, 0, 6, 3, 0, 6, 4, 0, 6, 5, 0, 6, 6, 0, 6, 7, 0, 6, 8, 0, 6, 9, 0, 6, 10, 0, 6, 11, 0, 6, 12, 0, 6, 13, 0, 6, 14, 0, 6, 15, 0, 7, 0, 0, 7, 1, 0, 7, 2, 0, 7, 3, 0, 7, 4, 0, 7, 5, 0, 7, 6, 0, 7, 7, 0, 7, 8, 0, 7, 9, 0, 7, 10, 0, 7, 11, 0, 7, 12, 0, 7, 13, 0, 7, 14, 0, 7, 15, 0 ]
						}
, 						{
							"number" : 4,
							"data" : [ 5, "obj-3", "number", "int", 10, 5, "obj-6", "toggle", "int", 1, 5, "obj-23", "number", "int", 10, 5, "obj-37", "toggle", "int", 0, 256, "obj-46", "matrixctrl", "list", 0, 0, 0, 0, 1, 0, 0, 2, 0, 0, 3, 0, 0, 4, 0, 0, 5, 0, 0, 6, 0, 0, 7, 0, 0, 8, 0, 0, 9, 0, 0, 10, 0, 0, 11, 0, 0, 12, 0, 0, 13, 0, 0, 14, 0, 0, 15, 0, 1, 0, 0, 1, 1, 0, 1, 2, 0, 1, 3, 0, 1, 4, 0, 1, 5, 0, 1, 6, 0, 1, 7, 0, 1, 8, 0, 1, 9, 0, 1, 10, 0, 1, 11, 0, 1, 12, 0, 1, 13, 0, 1, 14, 0, 1, 15, 0, 2, 0, 0, 2, 1, 0, 2, 2, 0, 2, 3, 0, 2, 4, 0, 2, 5, 0, 2, 6, 0, 2, 7, 0, 2, 8, 0, 2, 9, 0, 2, 10, 0, 2, 11, 0, 2, 12, 0, 2, 13, 0, 2, 14, 0, 2, 15, 0, 3, 0, 0, 3, 1, 0, 3, 2, 0, 3, 3, 0, 3, 4, 0, 3, 5, 0, 3, 6, 0, 3, 7, 0, 3, 8, 0, 3, 9, 0, 3, 10, 0, 3, 11, 0, 3, 12, 0, 3, 13, 0, 3, 14, 0, 3, 15, 0, 4, 0, 0, 4, 1, 0, 4, 2, 0, 4, 3, 0, 4, 4, 0, 4, 5, 0, 4, 6, 0, 4, 7, 0, 4, 8, 0, 4, 9, 0, 4, 10, 0, 4, 11, 0, 4, 12, 0, 4, 13, 0, 4, 14, 0, 4, 15, 0, 5, 0, 0, 5, 1, 0, 5, 2, 0, 5, 3, 0, 136, "obj-46", "matrixctrl", "list", 5, 4, 0, 5, 5, 0, 5, 6, 0, 5, 7, 0, 5, 8, 0, 5, 9, 0, 5, 10, 0, 5, 11, 0, 5, 12, 0, 5, 13, 0, 5, 14, 0, 5, 15, 0, 6, 0, 0, 6, 1, 0, 6, 2, 0, 6, 3, 0, 6, 4, 0, 6, 5, 0, 6, 6, 0, 6, 7, 0, 6, 8, 0, 6, 9, 0, 6, 10, 0, 6, 11, 0, 6, 12, 0, 6, 13, 0, 6, 14, 0, 6, 15, 0, 7, 0, 0, 7, 1, 0, 7, 2, 0, 7, 3, 0, 7, 4, 0, 7, 5, 0, 7, 6, 0, 7, 7, 0, 7, 8, 0, 7, 9, 0, 7, 10, 0, 7, 11, 0, 7, 12, 0, 7, 13, 0, 7, 14, 0, 7, 15, 0 ]
						}
 ]
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"varname" : "u767000377",
					"text" : "pattr",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontface" : 0,
					"fontsize" : 10.0,
					"background" : 0,
					"color" : [ 0.8, 0.84, 0.71, 1.0 ],
					"numinlets" : 1,
					"ignoreclick" : 0,
					"numoutlets" : 3,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"fontname" : "Arial",
					"outlettype" : [ "", "", "" ],
					"patching_rect" : [ 405.0, 195.0, 46.0, 18.0 ],
					"presentation" : 0,
					"id" : "obj-41",
					"restore" : [ 0 ],
					"saved_object_attributes" : 					{
						"parameter_enable" : 0
					}

				}

			}
, 			{
				"box" : 				{
					"maxclass" : "message",
					"text" : "0",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontface" : 0,
					"fontsize" : 10.0,
					"background" : 0,
					"bgcolor2" : [ 0.867, 0.867, 0.867, 1.0 ],
					"numinlets" : 2,
					"ignoreclick" : 0,
					"gradient" : 0,
					"numoutlets" : 1,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 0.867, 0.867, 0.867, 1.0 ],
					"fontname" : "Arial",
					"outlettype" : [ "" ],
					"patching_rect" : [ 334.0, 551.0, 27.0, 16.0 ],
					"presentation" : 0,
					"id" : "obj-40"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "s fromroxor_patterncopyto",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontface" : 0,
					"fontsize" : 10.0,
					"background" : 0,
					"color" : [ 0.8, 0.84, 0.71, 1.0 ],
					"numinlets" : 1,
					"ignoreclick" : 0,
					"numoutlets" : 0,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"fontname" : "Arial",
					"patching_rect" : [ 314.0, 605.0, 127.0, 18.0 ],
					"presentation" : 0,
					"id" : "obj-36"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "message",
					"text" : "1",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontface" : 0,
					"fontsize" : 10.0,
					"background" : 0,
					"bgcolor2" : [ 0.867, 0.867, 0.867, 1.0 ],
					"numinlets" : 2,
					"ignoreclick" : 0,
					"gradient" : 0,
					"numoutlets" : 1,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 0.867, 0.867, 0.867, 1.0 ],
					"fontname" : "Arial",
					"outlettype" : [ "" ],
					"patching_rect" : [ 314.0, 587.0, 27.0, 16.0 ],
					"presentation" : 0,
					"id" : "obj-35"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "s fromroxor_patterncopyfrom",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontface" : 0,
					"fontsize" : 10.0,
					"background" : 0,
					"color" : [ 0.8, 0.84, 0.71, 1.0 ],
					"numinlets" : 1,
					"ignoreclick" : 0,
					"numoutlets" : 0,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"fontname" : "Arial",
					"patching_rect" : [ 305.0, 625.0, 139.0, 18.0 ],
					"presentation" : 0,
					"id" : "obj-33"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "message",
					"text" : "0",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontface" : 0,
					"fontsize" : 10.0,
					"background" : 0,
					"bgcolor2" : [ 0.867, 0.867, 0.867, 1.0 ],
					"numinlets" : 2,
					"ignoreclick" : 0,
					"gradient" : 0,
					"numoutlets" : 1,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 0.867, 0.867, 0.867, 1.0 ],
					"fontname" : "Arial",
					"outlettype" : [ "" ],
					"patching_rect" : [ 306.0, 550.0, 27.0, 16.0 ],
					"presentation" : 0,
					"id" : "obj-34"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "message",
					"text" : "0",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontface" : 0,
					"fontsize" : 10.0,
					"background" : 0,
					"bgcolor2" : [ 0.867, 0.867, 0.867, 1.0 ],
					"numinlets" : 2,
					"ignoreclick" : 0,
					"gradient" : 0,
					"numoutlets" : 1,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 0.867, 0.867, 0.867, 1.0 ],
					"fontname" : "Arial",
					"outlettype" : [ "" ],
					"patching_rect" : [ 278.0, 550.0, 27.0, 16.0 ],
					"presentation" : 0,
					"id" : "obj-31"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "message",
					"text" : "0",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontface" : 0,
					"fontsize" : 10.0,
					"background" : 0,
					"bgcolor2" : [ 0.867, 0.867, 0.867, 1.0 ],
					"numinlets" : 2,
					"ignoreclick" : 0,
					"gradient" : 0,
					"numoutlets" : 1,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 0.867, 0.867, 0.867, 1.0 ],
					"fontname" : "Arial",
					"outlettype" : [ "" ],
					"patching_rect" : [ 251.0, 549.0, 27.0, 16.0 ],
					"presentation" : 0,
					"id" : "obj-29"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "message",
					"text" : "0",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontface" : 0,
					"fontsize" : 10.0,
					"background" : 0,
					"bgcolor2" : [ 0.867, 0.867, 0.867, 1.0 ],
					"numinlets" : 2,
					"ignoreclick" : 0,
					"gradient" : 0,
					"numoutlets" : 1,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 0.867, 0.867, 0.867, 1.0 ],
					"fontname" : "Arial",
					"outlettype" : [ "" ],
					"patching_rect" : [ 226.0, 549.0, 27.0, 16.0 ],
					"presentation" : 0,
					"id" : "obj-28"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "message",
					"text" : "0",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontface" : 0,
					"fontsize" : 10.0,
					"background" : 0,
					"bgcolor2" : [ 0.867, 0.867, 0.867, 1.0 ],
					"numinlets" : 2,
					"ignoreclick" : 0,
					"gradient" : 0,
					"numoutlets" : 1,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 0.867, 0.867, 0.867, 1.0 ],
					"fontname" : "Arial",
					"outlettype" : [ "" ],
					"patching_rect" : [ 179.0, 548.0, 27.0, 16.0 ],
					"presentation" : 0,
					"id" : "obj-25"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "message",
					"text" : "0",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontface" : 0,
					"fontsize" : 10.0,
					"background" : 0,
					"bgcolor2" : [ 0.867, 0.867, 0.867, 1.0 ],
					"numinlets" : 2,
					"ignoreclick" : 0,
					"gradient" : 0,
					"numoutlets" : 1,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 0.867, 0.867, 0.867, 1.0 ],
					"fontname" : "Arial",
					"outlettype" : [ "" ],
					"patching_rect" : [ 202.0, 548.0, 27.0, 16.0 ],
					"presentation" : 0,
					"id" : "obj-24"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "r stepnum",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontface" : 0,
					"fontsize" : 10.0,
					"background" : 0,
					"color" : [ 0.8, 0.84, 0.71, 1.0 ],
					"numinlets" : 0,
					"ignoreclick" : 0,
					"numoutlets" : 1,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"fontname" : "Arial",
					"outlettype" : [ "" ],
					"patching_rect" : [ 330.0, 15.0, 55.0, 18.0 ],
					"presentation" : 0,
					"id" : "obj-17"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "s fromroxor_play",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontface" : 0,
					"fontsize" : 10.0,
					"background" : 0,
					"color" : [ 0.8, 0.84, 0.71, 1.0 ],
					"numinlets" : 1,
					"ignoreclick" : 0,
					"numoutlets" : 0,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"fontname" : "Arial",
					"patching_rect" : [ 352.0, 584.0, 85.0, 18.0 ],
					"presentation" : 0,
					"id" : "obj-12"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "message",
					"text" : "0",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontface" : 0,
					"fontsize" : 12.0,
					"background" : 0,
					"bgcolor2" : [ 0.867, 0.867, 0.867, 1.0 ],
					"numinlets" : 2,
					"ignoreclick" : 0,
					"gradient" : 0,
					"numoutlets" : 1,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 0.867, 0.867, 0.867, 1.0 ],
					"fontname" : "Arial",
					"outlettype" : [ "" ],
					"patching_rect" : [ 588.0, 527.0, 57.0, 18.0 ],
					"presentation" : 0,
					"id" : "obj-11"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "button",
					"outlinecolor" : [ 0.71328, 0.71328, 0.71328, 1.0 ],
					"background" : 0,
					"blinkcolor" : [ 1.0, 0.890196, 0.090196, 1.0 ],
					"numinlets" : 1,
					"ignoreclick" : 0,
					"numoutlets" : 1,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 0.91328, 0.91328, 0.91328, 0.75 ],
					"outlettype" : [ "bang" ],
					"fgcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"patching_rect" : [ 564.0, 525.0, 20.0, 20.0 ],
					"presentation" : 0,
					"id" : "obj-70"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "fromsymbol @separator \\;",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontface" : 0,
					"fontsize" : 11.595187,
					"background" : 0,
					"color" : [ 0.356863, 0.768627, 0.0, 1.0 ],
					"numinlets" : 1,
					"ignoreclick" : 0,
					"numoutlets" : 1,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"fontname" : "Arial",
					"outlettype" : [ "" ],
					"patching_rect" : [ 195.0, 285.0, 144.0, 20.0 ],
					"presentation" : 0,
					"id" : "obj-69"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "button",
					"outlinecolor" : [ 0.71328, 0.71328, 0.71328, 1.0 ],
					"background" : 0,
					"blinkcolor" : [ 1.0, 0.890196, 0.090196, 1.0 ],
					"numinlets" : 1,
					"ignoreclick" : 0,
					"numoutlets" : 1,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 0.91328, 0.91328, 0.91328, 0.75 ],
					"outlettype" : [ "bang" ],
					"fgcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"patching_rect" : [ 600.0, 390.0, 67.0, 67.0 ],
					"presentation" : 0,
					"id" : "obj-68"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "button",
					"outlinecolor" : [ 0.71328, 0.71328, 0.71328, 1.0 ],
					"background" : 0,
					"blinkcolor" : [ 1.0, 0.890196, 0.090196, 1.0 ],
					"numinlets" : 1,
					"ignoreclick" : 0,
					"numoutlets" : 1,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 0.91328, 0.91328, 0.91328, 0.75 ],
					"outlettype" : [ "bang" ],
					"fgcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"patching_rect" : [ 540.0, 526.0, 20.0, 20.0 ],
					"presentation" : 0,
					"id" : "obj-64"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "button",
					"outlinecolor" : [ 0.71328, 0.71328, 0.71328, 1.0 ],
					"background" : 0,
					"blinkcolor" : [ 1.0, 0.890196, 0.090196, 1.0 ],
					"numinlets" : 1,
					"ignoreclick" : 0,
					"numoutlets" : 1,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 0.91328, 0.91328, 0.91328, 0.75 ],
					"outlettype" : [ "bang" ],
					"fgcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"patching_rect" : [ 516.0, 525.0, 20.0, 20.0 ],
					"presentation" : 0,
					"id" : "obj-65"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "button",
					"outlinecolor" : [ 0.71328, 0.71328, 0.71328, 1.0 ],
					"background" : 0,
					"blinkcolor" : [ 1.0, 0.890196, 0.090196, 1.0 ],
					"numinlets" : 1,
					"ignoreclick" : 0,
					"numoutlets" : 1,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 0.91328, 0.91328, 0.91328, 0.75 ],
					"outlettype" : [ "bang" ],
					"fgcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"patching_rect" : [ 492.0, 525.0, 20.0, 20.0 ],
					"presentation" : 0,
					"id" : "obj-66"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "button",
					"outlinecolor" : [ 0.71328, 0.71328, 0.71328, 1.0 ],
					"background" : 0,
					"blinkcolor" : [ 1.0, 0.890196, 0.090196, 1.0 ],
					"numinlets" : 1,
					"ignoreclick" : 0,
					"numoutlets" : 1,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 0.91328, 0.91328, 0.91328, 0.75 ],
					"outlettype" : [ "bang" ],
					"fgcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"patching_rect" : [ 468.0, 525.0, 20.0, 20.0 ],
					"presentation" : 0,
					"id" : "obj-67"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "button",
					"outlinecolor" : [ 0.71328, 0.71328, 0.71328, 1.0 ],
					"background" : 0,
					"blinkcolor" : [ 1.0, 0.890196, 0.090196, 1.0 ],
					"numinlets" : 1,
					"ignoreclick" : 0,
					"numoutlets" : 1,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 0.91328, 0.91328, 0.91328, 0.75 ],
					"outlettype" : [ "bang" ],
					"fgcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"patching_rect" : [ 444.0, 525.0, 20.0, 20.0 ],
					"presentation" : 0,
					"id" : "obj-60"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "button",
					"outlinecolor" : [ 0.71328, 0.71328, 0.71328, 1.0 ],
					"background" : 0,
					"blinkcolor" : [ 1.0, 0.890196, 0.090196, 1.0 ],
					"numinlets" : 1,
					"ignoreclick" : 0,
					"numoutlets" : 1,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 0.91328, 0.91328, 0.91328, 0.75 ],
					"outlettype" : [ "bang" ],
					"fgcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"patching_rect" : [ 420.0, 525.0, 20.0, 20.0 ],
					"presentation" : 0,
					"id" : "obj-61"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "button",
					"outlinecolor" : [ 0.71328, 0.71328, 0.71328, 1.0 ],
					"background" : 0,
					"blinkcolor" : [ 1.0, 0.890196, 0.090196, 1.0 ],
					"numinlets" : 1,
					"ignoreclick" : 0,
					"numoutlets" : 1,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 0.91328, 0.91328, 0.91328, 0.75 ],
					"outlettype" : [ "bang" ],
					"fgcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"patching_rect" : [ 396.0, 525.0, 20.0, 20.0 ],
					"presentation" : 0,
					"id" : "obj-62"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "button",
					"outlinecolor" : [ 0.71328, 0.71328, 0.71328, 1.0 ],
					"background" : 0,
					"blinkcolor" : [ 1.0, 0.890196, 0.090196, 1.0 ],
					"numinlets" : 1,
					"ignoreclick" : 0,
					"numoutlets" : 1,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 0.91328, 0.91328, 0.91328, 0.75 ],
					"outlettype" : [ "bang" ],
					"fgcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"patching_rect" : [ 372.0, 525.0, 20.0, 20.0 ],
					"presentation" : 0,
					"id" : "obj-63"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "button",
					"outlinecolor" : [ 0.71328, 0.71328, 0.71328, 1.0 ],
					"background" : 0,
					"blinkcolor" : [ 1.0, 0.890196, 0.090196, 1.0 ],
					"numinlets" : 1,
					"ignoreclick" : 0,
					"numoutlets" : 1,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 0.91328, 0.91328, 0.91328, 0.75 ],
					"outlettype" : [ "bang" ],
					"fgcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"patching_rect" : [ 348.0, 525.0, 20.0, 20.0 ],
					"presentation" : 0,
					"id" : "obj-56"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "button",
					"outlinecolor" : [ 0.71328, 0.71328, 0.71328, 1.0 ],
					"background" : 0,
					"blinkcolor" : [ 1.0, 0.890196, 0.090196, 1.0 ],
					"numinlets" : 1,
					"ignoreclick" : 0,
					"numoutlets" : 1,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 0.91328, 0.91328, 0.91328, 0.75 ],
					"outlettype" : [ "bang" ],
					"fgcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"patching_rect" : [ 324.0, 525.0, 20.0, 20.0 ],
					"presentation" : 0,
					"id" : "obj-57"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "button",
					"outlinecolor" : [ 0.71328, 0.71328, 0.71328, 1.0 ],
					"background" : 0,
					"blinkcolor" : [ 1.0, 0.890196, 0.090196, 1.0 ],
					"numinlets" : 1,
					"ignoreclick" : 0,
					"numoutlets" : 1,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 0.91328, 0.91328, 0.91328, 0.75 ],
					"outlettype" : [ "bang" ],
					"fgcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"patching_rect" : [ 300.0, 525.0, 20.0, 20.0 ],
					"presentation" : 0,
					"id" : "obj-58"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "button",
					"outlinecolor" : [ 0.71328, 0.71328, 0.71328, 1.0 ],
					"background" : 0,
					"blinkcolor" : [ 1.0, 0.890196, 0.090196, 1.0 ],
					"numinlets" : 1,
					"ignoreclick" : 0,
					"numoutlets" : 1,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 0.91328, 0.91328, 0.91328, 0.75 ],
					"outlettype" : [ "bang" ],
					"fgcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"patching_rect" : [ 276.0, 525.0, 20.0, 20.0 ],
					"presentation" : 0,
					"id" : "obj-59"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "button",
					"outlinecolor" : [ 0.71328, 0.71328, 0.71328, 1.0 ],
					"background" : 0,
					"blinkcolor" : [ 1.0, 0.890196, 0.090196, 1.0 ],
					"numinlets" : 1,
					"ignoreclick" : 0,
					"numoutlets" : 1,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 0.91328, 0.91328, 0.91328, 0.75 ],
					"outlettype" : [ "bang" ],
					"fgcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"patching_rect" : [ 252.0, 525.0, 20.0, 20.0 ],
					"presentation" : 0,
					"id" : "obj-54"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "button",
					"outlinecolor" : [ 0.71328, 0.71328, 0.71328, 1.0 ],
					"background" : 0,
					"blinkcolor" : [ 1.0, 0.890196, 0.090196, 1.0 ],
					"numinlets" : 1,
					"ignoreclick" : 0,
					"numoutlets" : 1,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 0.91328, 0.91328, 0.91328, 0.75 ],
					"outlettype" : [ "bang" ],
					"fgcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"patching_rect" : [ 228.0, 525.0, 20.0, 20.0 ],
					"presentation" : 0,
					"id" : "obj-55"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "message",
					"text" : "0",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontface" : 0,
					"fontsize" : 12.0,
					"background" : 0,
					"bgcolor2" : [ 0.867, 0.867, 0.867, 1.0 ],
					"numinlets" : 2,
					"ignoreclick" : 0,
					"gradient" : 0,
					"numoutlets" : 1,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 0.867, 0.867, 0.867, 1.0 ],
					"fontname" : "Arial",
					"outlettype" : [ "" ],
					"patching_rect" : [ 140.0, 515.0, 23.0, 18.0 ],
					"presentation" : 0,
					"id" : "obj-53"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "message",
					"text" : "0",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontface" : 0,
					"fontsize" : 12.0,
					"background" : 0,
					"bgcolor2" : [ 0.867, 0.867, 0.867, 1.0 ],
					"numinlets" : 2,
					"ignoreclick" : 0,
					"gradient" : 0,
					"numoutlets" : 1,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 0.867, 0.867, 0.867, 1.0 ],
					"fontname" : "Arial",
					"outlettype" : [ "" ],
					"patching_rect" : [ 115.0, 516.0, 24.0, 18.0 ],
					"presentation" : 0,
					"id" : "obj-52"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "message",
					"text" : "1",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontface" : 0,
					"fontsize" : 12.0,
					"background" : 0,
					"bgcolor2" : [ 0.867, 0.867, 0.867, 1.0 ],
					"numinlets" : 2,
					"ignoreclick" : 0,
					"gradient" : 0,
					"numoutlets" : 1,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 0.867, 0.867, 0.867, 1.0 ],
					"fontname" : "Arial",
					"outlettype" : [ "" ],
					"patching_rect" : [ 88.0, 516.0, 25.0, 18.0 ],
					"presentation" : 0,
					"id" : "obj-51"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "button",
					"outlinecolor" : [ 0.71328, 0.71328, 0.71328, 1.0 ],
					"background" : 0,
					"blinkcolor" : [ 1.0, 0.890196, 0.090196, 1.0 ],
					"numinlets" : 1,
					"ignoreclick" : 0,
					"numoutlets" : 1,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 0.91328, 0.91328, 0.91328, 0.75 ],
					"outlettype" : [ "bang" ],
					"fgcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"patching_rect" : [ 204.0, 525.0, 20.0, 20.0 ],
					"presentation" : 0,
					"id" : "obj-49"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "button",
					"outlinecolor" : [ 0.71328, 0.71328, 0.71328, 1.0 ],
					"background" : 0,
					"blinkcolor" : [ 1.0, 0.890196, 0.090196, 1.0 ],
					"numinlets" : 1,
					"ignoreclick" : 0,
					"numoutlets" : 1,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 0.91328, 0.91328, 0.91328, 0.75 ],
					"outlettype" : [ "bang" ],
					"fgcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"patching_rect" : [ 180.0, 525.0, 20.0, 20.0 ],
					"presentation" : 0,
					"id" : "obj-48"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "select ZTA ZTM ZOC ZSW ZPS ZPC ZPL ZMU ZSO ZVL ZRV ZCC ZRC ZNN ZRN ZMC ZSD",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontface" : 0,
					"fontsize" : 10.0,
					"background" : 0,
					"color" : [ 0.8, 0.84, 0.71, 1.0 ],
					"numinlets" : 1,
					"ignoreclick" : 0,
					"numoutlets" : 18,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"fontname" : "Arial",
					"outlettype" : [ "bang", "bang", "bang", "bang", "bang", "bang", "bang", "bang", "bang", "bang", "bang", "bang", "bang", "bang", "bang", "bang", "bang", "" ],
					"patching_rect" : [ 180.0, 468.0, 427.0, 18.0 ],
					"presentation" : 0,
					"id" : "obj-47"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "message",
					"text" : "0",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontface" : 0,
					"fontsize" : 12.0,
					"background" : 0,
					"bgcolor2" : [ 0.867, 0.867, 0.867, 1.0 ],
					"numinlets" : 2,
					"ignoreclick" : 0,
					"gradient" : 0,
					"numoutlets" : 1,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 0.867, 0.867, 0.867, 1.0 ],
					"fontname" : "Arial",
					"outlettype" : [ "" ],
					"patching_rect" : [ 53.0, 515.0, 33.0, 18.0 ],
					"presentation" : 0,
					"id" : "obj-45"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "message",
					"text" : "ZSD",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontface" : 0,
					"fontsize" : 12.0,
					"background" : 0,
					"bgcolor2" : [ 0.867, 0.867, 0.867, 1.0 ],
					"numinlets" : 2,
					"ignoreclick" : 0,
					"gradient" : 0,
					"numoutlets" : 1,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 0.867, 0.867, 0.867, 1.0 ],
					"fontname" : "Arial",
					"outlettype" : [ "" ],
					"patching_rect" : [ 4.0, 516.0, 47.0, 18.0 ],
					"presentation" : 0,
					"id" : "obj-44"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "unpack s i i i i i i",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontface" : 0,
					"fontsize" : 11.595187,
					"background" : 0,
					"color" : [ 0.8, 0.84, 0.71, 1.0 ],
					"numinlets" : 1,
					"ignoreclick" : 0,
					"numoutlets" : 7,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"fontname" : "Arial",
					"outlettype" : [ "", "int", "int", "int", "int", "int", "int" ],
					"patching_rect" : [ 195.0, 363.0, 100.0, 20.0 ],
					"presentation" : 0,
					"id" : "obj-10"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "fromsymbol @separator \\,",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontface" : 0,
					"fontsize" : 11.595187,
					"background" : 0,
					"color" : [ 0.356863, 0.768627, 0.0, 1.0 ],
					"numinlets" : 1,
					"ignoreclick" : 0,
					"numoutlets" : 1,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"fontname" : "Arial",
					"outlettype" : [ "" ],
					"patching_rect" : [ 195.0, 330.0, 144.0, 20.0 ],
					"presentation" : 0,
					"id" : "obj-9"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "comment",
					"text" : "3",
					"underline" : 0,
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontface" : 0,
					"fontsize" : 11.595187,
					"background" : 0,
					"numinlets" : 1,
					"ignoreclick" : 1,
					"numoutlets" : 0,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 1.0, 1.0, 1.0, 0.0 ],
					"frgb" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontname" : "Arial",
					"patching_rect" : [ 52.0, 213.0, 19.0, 20.0 ],
					"presentation" : 0,
					"id" : "obj-42"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "toggle",
					"background" : 0,
					"numinlets" : 1,
					"ignoreclick" : 0,
					"numoutlets" : 1,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"checkedcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"outlettype" : [ "int" ],
					"bordercolor" : [ 0.5, 0.5, 0.5, 1.0 ],
					"patching_rect" : [ 30.0, 213.0, 20.0, 20.0 ],
					"presentation" : 0,
					"id" : "obj-37"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "print formatted",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontface" : 0,
					"fontsize" : 11.595187,
					"background" : 0,
					"color" : [ 0.8, 0.84, 0.71, 1.0 ],
					"numinlets" : 1,
					"ignoreclick" : 0,
					"numoutlets" : 0,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"fontname" : "Arial",
					"patching_rect" : [ 30.0, 270.0, 85.0, 20.0 ],
					"presentation" : 0,
					"id" : "obj-38"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "gate",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontface" : 0,
					"fontsize" : 11.595187,
					"background" : 0,
					"color" : [ 0.8, 0.84, 0.71, 1.0 ],
					"numinlets" : 2,
					"ignoreclick" : 0,
					"numoutlets" : 1,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"fontname" : "Arial",
					"outlettype" : [ "" ],
					"patching_rect" : [ 30.0, 240.0, 33.0, 20.0 ],
					"presentation" : 0,
					"id" : "obj-39"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "number",
					"maximum" : "<none>",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontface" : 0,
					"fontsize" : 11.595187,
					"background" : 0,
					"hbgcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"mouseup" : 0,
					"cantchange" : 0,
					"triangle" : 1,
					"tricolor" : [ 0.75, 0.75, 0.75, 1.0 ],
					"minimum" : "<none>",
					"numinlets" : 1,
					"htextcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"outputonclick" : 0,
					"ignoreclick" : 0,
					"numoutlets" : 2,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"format" : 0,
					"fontname" : "Arial",
					"outlettype" : [ "int", "bang" ],
					"bordercolor" : [ 0.5, 0.5, 0.5, 1.0 ],
					"patching_rect" : [ 195.0, 119.0, 50.0, 20.0 ],
					"presentation" : 0,
					"id" : "obj-23",
					"htricolor" : [ 0.87, 0.82, 0.24, 1.0 ],
					"triscale" : 1.0
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "comment",
					"text" : "< start polling the serial port",
					"underline" : 0,
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontface" : 0,
					"fontsize" : 11.595187,
					"background" : 0,
					"numinlets" : 1,
					"ignoreclick" : 1,
					"numoutlets" : 0,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 1.0, 1.0, 1.0, 0.0 ],
					"frgb" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontname" : "Arial",
					"patching_rect" : [ 147.0, 27.0, 153.0, 20.0 ],
					"presentation" : 0,
					"id" : "obj-18"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "comment",
					"text" : "< send a byte to start the transmission",
					"underline" : 0,
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontface" : 0,
					"fontsize" : 11.595187,
					"background" : 0,
					"numinlets" : 1,
					"ignoreclick" : 1,
					"numoutlets" : 0,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 1.0, 1.0, 1.0, 0.0 ],
					"frgb" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontname" : "Arial",
					"patching_rect" : [ 300.0, 60.0, 207.0, 20.0 ],
					"presentation" : 0,
					"id" : "obj-14"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "itoa",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontface" : 0,
					"fontsize" : 11.595187,
					"background" : 0,
					"color" : [ 0.356863, 0.768627, 0.0, 1.0 ],
					"numinlets" : 3,
					"ignoreclick" : 0,
					"numoutlets" : 1,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"fontname" : "Arial",
					"outlettype" : [ "int" ],
					"patching_rect" : [ 195.0, 225.0, 46.0, 20.0 ],
					"presentation" : 0,
					"id" : "obj-8"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "zl group 32",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontface" : 0,
					"fontsize" : 11.595187,
					"background" : 0,
					"color" : [ 0.8, 0.84, 0.71, 1.0 ],
					"numinlets" : 2,
					"ignoreclick" : 0,
					"numoutlets" : 2,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"fontname" : "Arial",
					"outlettype" : [ "", "" ],
					"patching_rect" : [ 195.0, 195.0, 68.0, 20.0 ],
					"presentation" : 0,
					"id" : "obj-7"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "toggle",
					"background" : 0,
					"numinlets" : 1,
					"ignoreclick" : 0,
					"numoutlets" : 1,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"checkedcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"outlettype" : [ "int" ],
					"bordercolor" : [ 0.5, 0.5, 0.5, 1.0 ],
					"patching_rect" : [ 117.0, 27.0, 20.0, 20.0 ],
					"presentation" : 0,
					"id" : "obj-6"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "metro 100",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontface" : 0,
					"fontsize" : 11.595187,
					"background" : 0,
					"color" : [ 0.8, 0.84, 0.71, 1.0 ],
					"numinlets" : 2,
					"ignoreclick" : 0,
					"numoutlets" : 1,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"fontname" : "Arial",
					"outlettype" : [ "bang" ],
					"patching_rect" : [ 119.0, 54.0, 64.0, 20.0 ],
					"presentation" : 0,
					"id" : "obj-5"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "sel 13 10",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontface" : 0,
					"fontsize" : 11.595187,
					"background" : 0,
					"color" : [ 0.8, 0.84, 0.71, 1.0 ],
					"numinlets" : 1,
					"ignoreclick" : 0,
					"numoutlets" : 3,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"fontname" : "Arial",
					"outlettype" : [ "bang", "bang", "" ],
					"patching_rect" : [ 195.0, 152.0, 58.0, 20.0 ],
					"presentation" : 0,
					"id" : "obj-4"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "number",
					"maximum" : "<none>",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontface" : 0,
					"fontsize" : 11.595187,
					"background" : 0,
					"hbgcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"mouseup" : 0,
					"cantchange" : 0,
					"triangle" : 1,
					"tricolor" : [ 0.75, 0.75, 0.75, 1.0 ],
					"minimum" : "<none>",
					"numinlets" : 1,
					"htextcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"outputonclick" : 0,
					"ignoreclick" : 0,
					"numoutlets" : 2,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"format" : 0,
					"fontname" : "Arial",
					"outlettype" : [ "int", "bang" ],
					"bordercolor" : [ 0.5, 0.5, 0.5, 1.0 ],
					"patching_rect" : [ 243.0, 54.0, 50.0, 20.0 ],
					"presentation" : 0,
					"id" : "obj-3",
					"htricolor" : [ 0.87, 0.82, 0.24, 1.0 ],
					"triscale" : 1.0
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "message",
					"text" : "print",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontface" : 0,
					"fontsize" : 11.595187,
					"background" : 0,
					"bgcolor2" : [ 0.867, 0.867, 0.867, 1.0 ],
					"numinlets" : 2,
					"ignoreclick" : 0,
					"gradient" : 0,
					"numoutlets" : 1,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 0.867, 0.867, 0.867, 1.0 ],
					"fontname" : "Arial",
					"outlettype" : [ "" ],
					"patching_rect" : [ 195.0, 54.0, 33.0, 18.0 ],
					"presentation" : 0,
					"id" : "obj-2"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "serial a 57600",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"fontface" : 0,
					"fontsize" : 11.595187,
					"background" : 0,
					"color" : [ 0.356863, 0.768627, 0.0, 1.0 ],
					"numinlets" : 1,
					"ignoreclick" : 0,
					"numoutlets" : 2,
					"presentation_rect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"hidden" : 0,
					"bgcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"fontname" : "Arial",
					"outlettype" : [ "int", "" ],
					"patching_rect" : [ 195.0, 88.0, 83.0, 20.0 ],
					"presentation" : 0,
					"id" : "obj-1"
				}

			}
 ],
		"lines" : [ 			{
				"patchline" : 				{
					"source" : [ "obj-71", 0 ],
					"destination" : [ "obj-95", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-95", 0 ],
					"destination" : [ "obj-118", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-13", 0 ],
					"destination" : [ "obj-71", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-117", 0 ],
					"destination" : [ "obj-13", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-91", 0 ],
					"destination" : [ "obj-85", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-89", 0 ],
					"destination" : [ "obj-85", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-87", 0 ],
					"destination" : [ "obj-85", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-82", 0 ],
					"destination" : [ "obj-85", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-4", 0 ],
					"destination" : [ "obj-7", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-4", 2 ],
					"destination" : [ "obj-7", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [ 243.5, 187.0, 207.0, 187.0, 207.0, 192.0, 204.5, 192.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-7", 0 ],
					"destination" : [ "obj-8", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-84", 0 ],
					"destination" : [ "obj-107", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-84", 0 ],
					"destination" : [ "obj-77", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-5", 0 ],
					"destination" : [ "obj-1", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-2", 0 ],
					"destination" : [ "obj-1", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-3", 0 ],
					"destination" : [ "obj-1", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-1", 0 ],
					"destination" : [ "obj-23", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-10", 2 ],
					"destination" : [ "obj-72", 1 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-40", 0 ],
					"destination" : [ "obj-12", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-57", 0 ],
					"destination" : [ "obj-40", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-10", 1 ],
					"destination" : [ "obj-40", 1 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-35", 0 ],
					"destination" : [ "obj-36", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-10", 2 ],
					"destination" : [ "obj-35", 1 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-10", 1 ],
					"destination" : [ "obj-34", 1 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-58", 0 ],
					"destination" : [ "obj-35", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-34", 0 ],
					"destination" : [ "obj-33", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-10", 1 ],
					"destination" : [ "obj-31", 1 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-58", 0 ],
					"destination" : [ "obj-34", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-59", 0 ],
					"destination" : [ "obj-31", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-10", 1 ],
					"destination" : [ "obj-29", 1 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-54", 0 ],
					"destination" : [ "obj-29", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-48", 0 ],
					"destination" : [ "obj-25", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-55", 0 ],
					"destination" : [ "obj-28", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-10", 1 ],
					"destination" : [ "obj-28", 1 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-10", 1 ],
					"destination" : [ "obj-25", 1 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-49", 0 ],
					"destination" : [ "obj-24", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-10", 1 ],
					"destination" : [ "obj-24", 1 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-17", 0 ],
					"destination" : [ "obj-3", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-47", 17 ],
					"destination" : [ "obj-11", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-8", 0 ],
					"destination" : [ "obj-69", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-10", 4 ],
					"destination" : [ "obj-53", 1 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-10", 3 ],
					"destination" : [ "obj-52", 1 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-10", 2 ],
					"destination" : [ "obj-51", 1 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-9", 0 ],
					"destination" : [ "obj-10", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-10", 0 ],
					"destination" : [ "obj-44", 1 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-10", 1 ],
					"destination" : [ "obj-45", 1 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-10", 0 ],
					"destination" : [ "obj-47", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-47", 15 ],
					"destination" : [ "obj-64", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-47", 14 ],
					"destination" : [ "obj-65", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-47", 13 ],
					"destination" : [ "obj-66", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-47", 12 ],
					"destination" : [ "obj-67", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-47", 11 ],
					"destination" : [ "obj-60", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-47", 10 ],
					"destination" : [ "obj-61", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-47", 9 ],
					"destination" : [ "obj-62", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-47", 8 ],
					"destination" : [ "obj-63", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-47", 7 ],
					"destination" : [ "obj-56", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-47", 6 ],
					"destination" : [ "obj-57", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-47", 5 ],
					"destination" : [ "obj-58", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-47", 4 ],
					"destination" : [ "obj-59", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-47", 3 ],
					"destination" : [ "obj-54", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-47", 2 ],
					"destination" : [ "obj-55", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-47", 0 ],
					"destination" : [ "obj-48", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-47", 1 ],
					"destination" : [ "obj-49", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-69", 0 ],
					"destination" : [ "obj-9", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-47", 16 ],
					"destination" : [ "obj-70", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-47", 17 ],
					"destination" : [ "obj-68", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-8", 0 ],
					"destination" : [ "obj-39", 1 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [ 204.5, 249.0, 75.0, 249.0, 75.0, 237.0, 53.5, 237.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-39", 0 ],
					"destination" : [ "obj-38", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-37", 0 ],
					"destination" : [ "obj-39", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-23", 0 ],
					"destination" : [ "obj-4", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-6", 0 ],
					"destination" : [ "obj-5", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-46", 1 ],
					"destination" : [ "obj-15", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-16", 0 ],
					"destination" : [ "obj-43", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-19", 0 ],
					"destination" : [ "obj-43", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-20", 0 ],
					"destination" : [ "obj-43", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-22", 0 ],
					"destination" : [ "obj-43", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-31", 0 ],
					"destination" : [ "obj-73", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-73", 0 ],
					"destination" : [ "obj-43", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-74", 0 ],
					"destination" : [ "obj-43", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-75", 0 ],
					"destination" : [ "obj-43", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-24", 0 ],
					"destination" : [ "obj-79", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-49", 0 ],
					"destination" : [ "obj-79", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-25", 0 ],
					"destination" : [ "obj-82", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-48", 0 ],
					"destination" : [ "obj-82", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-79", 0 ],
					"destination" : [ "obj-85", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-28", 0 ],
					"destination" : [ "obj-87", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-55", 0 ],
					"destination" : [ "obj-87", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-29", 0 ],
					"destination" : [ "obj-89", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-54", 0 ],
					"destination" : [ "obj-89", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-31", 0 ],
					"destination" : [ "obj-91", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-59", 0 ],
					"destination" : [ "obj-91", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-72", 0 ],
					"destination" : [ "obj-50", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-50", 0 ],
					"destination" : [ "obj-46", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-10", 3 ],
					"destination" : [ "obj-50", 1 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-10", 4 ],
					"destination" : [ "obj-50", 2 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-72", 0 ],
					"destination" : [ "obj-117", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-10", 3 ],
					"destination" : [ "obj-117", 1 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-118", 0 ],
					"destination" : [ "obj-115", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-10", 4 ],
					"destination" : [ "obj-121", 1 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-121", 0 ],
					"destination" : [ "obj-110", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-115", 0 ],
					"destination" : [ "obj-121", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-79", 0 ],
					"destination" : [ "obj-124", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-124", 0 ],
					"destination" : [ "obj-120", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-122", 3 ],
					"destination" : [ "obj-115", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-122", 2 ],
					"destination" : [ "obj-118", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-70", 0 ],
					"destination" : [ "obj-122", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-122", 0 ],
					"destination" : [ "obj-72", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-122", 4 ],
					"destination" : [ "obj-124", 0 ],
					"hidden" : 0,
					"color" : [ 0.0, 0.0, 0.0, 1.0 ],
					"midpoints" : [  ]
				}

			}
 ],
		"parameters" : 		{

		}

	}

}
