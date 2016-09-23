<?xml version="1.0"?>
<root xmlns="http://www.vips.ecs.soton.ac.uk/nip/8.4.0">
  <Workspace window_x="0" window_y="0" window_width="948" window_height="694" filename="$HOME/GIT/nip2/share/nip2/data/examples/businesscard/businesscard.ws" view="WORKSPACE_MODE_REGULAR" scale="1" offset="0" lpane_position="400" lpane_open="false" rpane_position="100" rpane_open="false" local_defs="// private definitions for this workspace&#10;" name="businesscard" caption="Default empty workspace">
    <Column x="0" y="0" open="true" selected="true" sform="false" next="3" name="A" caption="source images">
      <Subcolumn vislevel="3">
        <Row popup="false" name="A1">
          <Rhs vislevel="1" flags="1">
            <iImage window_x="1323" window_y="513" window_width="590" window_height="650" image_left="574" image_top="570" image_mag="-2" show_status="true" show_paintbox="false" show_convert="false" show_rulers="false" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Image_file &quot;$HOME/GIT/nip2/share/nip2/data/examples/businesscard/slanted_oval_vase2.jpg&quot;"/>
          </Rhs>
        </Row>
        <Row popup="false" name="A2">
          <Rhs vislevel="1" flags="1">
            <iRegion image_left="-1" image_top="-1" image_mag="-1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="-1" offset="-1" falsecolour="false" type="true" left="52" top="74" width="406" height="614">
              <iRegiongroup/>
            </iRegion>
            <Subcolumn vislevel="0"/>
            <iText formula="Region A1 36 74 422 614"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="460" y="0" open="true" selected="false" sform="false" next="9" name="B" caption="text">
      <Subcolumn vislevel="3">
        <Row popup="false" name="B1">
          <Rhs vislevel="3" flags="7">
            <iImage window_x="174" window_y="155" window_width="1240" window_height="824" image_left="516" image_top="196" image_mag="8" show_status="true" show_paintbox="false" show_convert="false" show_rulers="false" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1">
              <Row name="super">
                <Rhs vislevel="0" flags="4">
                  <iImage image_left="-1" image_top="-1" image_mag="-1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="-1" offset="-1" falsecolour="false" type="true"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="text">
                <Rhs vislevel="1" flags="1">
                  <String caption="Text to paint" value="&lt;span size=&quot;xx-large&quot;&gt;Susan Green&lt;/span&gt;&#10;&#10;&lt;span size=&quot;large&quot;&gt;Stoneware Pottery&lt;/span&gt;&#10;&lt;i&gt;Commissions undertaken&lt;/i&gt;&#10;&#10;69 Tynestone Road, Cambridge&#10;01223 356051&#10;susangreen@waitrose.com&#10;http://susangreen.co.uk"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="font">
                <Rhs vislevel="1" flags="1">
                  <Fontname/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="wrap">
                <Rhs vislevel="1" flags="1">
                  <Subcolumn vislevel="0">
                    <Row name="caption">
                      <Rhs vislevel="0" flags="4">
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="expr">
                      <Rhs vislevel="0" flags="4">
                        <iText formula="1000"/>
                      </Rhs>
                    </Row>
                    <Row name="super">
                      <Rhs vislevel="1" flags="4">
                        <Subcolumn vislevel="0"/>
                        <iText/>
                      </Rhs>
                    </Row>
                  </Subcolumn>
                  <iText/>
                  <Expression caption="Wrap text at"/>
                </Rhs>
              </Row>
              <Row name="align">
                <Rhs vislevel="1" flags="1">
                  <Option caption="Alignment" labelsn="3" labels0="Right" labels1="Centre" labels2="Left" value="1"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="dpi">
                <Rhs vislevel="1" flags="1">
                  <Subcolumn vislevel="0"/>
                  <iText/>
                  <Expression caption="DPI"/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Pattern_images_item.Text_item.action"/>
          </Rhs>
        </Row>
        <Row popup="false" name="B5">
          <Rhs vislevel="1" flags="4">
            <iText formula="&quot;text colour&quot;"/>
          </Rhs>
        </Row>
        <Row popup="false" name="B7">
          <Rhs vislevel="3" flags="7">
            <Colour colour_space="sRGB" value0="86" value1="86" value2="86"/>
            <Subcolumn vislevel="1">
              <Row name="default_colour">
                <Rhs vislevel="0" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="default_value">
                <Rhs vislevel="0" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="super">
                <Rhs vislevel="0" flags="4">
                  <Colour/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="space">
                <Rhs vislevel="1" flags="1">
                  <Option caption="Colour space" labelsn="6" labels0="sRGB" labels1="Lab" labels2="LCh" labels3="XYZ" labels4="Yxy" labels5="UCS" value="0"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="colour">
                <Rhs vislevel="1" flags="1">
                  <Expression caption="Colour value"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Colour_new_item.Widget_colour_item.action"/>
          </Rhs>
        </Row>
        <Row popup="false" name="B6">
          <Rhs vislevel="1" flags="4">
            <iText formula="&quot;paper colour&quot;"/>
          </Rhs>
        </Row>
        <Row popup="false" name="B8">
          <Rhs vislevel="3" flags="7">
            <Colour colour_space="sRGB" value0="255" value1="255" value2="255"/>
            <Subcolumn vislevel="1">
              <Row name="default_colour">
                <Rhs vislevel="0" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="default_value">
                <Rhs vislevel="0" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="super">
                <Rhs vislevel="0" flags="4">
                  <Colour/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="space">
                <Rhs vislevel="1" flags="1">
                  <Option caption="Colour space" labelsn="6" labels0="sRGB" labels1="Lab" labels2="LCh" labels3="XYZ" labels4="Yxy" labels5="UCS" value="0"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="colour">
                <Rhs vislevel="1" flags="1">
                  <Expression caption="Colour value"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Colour_new_item.Widget_colour_item.action"/>
          </Rhs>
        </Row>
        <Row popup="false" name="B4">
          <Rhs vislevel="2" flags="5">
            <iImage window_x="925" window_y="84" window_width="923" window_height="920" image_left="453" image_top="420" image_mag="1" show_status="true" show_paintbox="false" show_convert="false" show_rulers="false" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Filter_blend_item.Image_blend_item.action B1 B7 B8"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="899" y="0" open="true" selected="false" sform="false" next="7" name="C" caption="make card">
      <Subcolumn vislevel="3">
        <Row popup="false" name="C1">
          <Rhs vislevel="1" flags="1">
            <iRegion image_left="-1" image_top="-1" image_mag="-1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="-1" offset="-1" falsecolour="false" type="true">
              <iRegiongroup/>
            </iRegion>
            <Subcolumn vislevel="0"/>
            <iText formula="A2"/>
          </Rhs>
        </Row>
        <Row popup="false" name="C2">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="-1" image_top="-1" image_mag="-1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="-1" offset="-1" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="B4"/>
          </Rhs>
        </Row>
        <Row popup="false" name="C6">
          <Rhs vislevel="1" flags="4">
            <iText formula="&quot;Margin:&quot;"/>
          </Rhs>
        </Row>
        <Row popup="false" name="C3">
          <Rhs vislevel="2" flags="4">
            <iText formula="40"/>
          </Rhs>
        </Row>
        <Row popup="false" name="C4">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="-1" image_top="-1" image_mag="-1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="-1" offset="-1" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1">
              <Row name="a">
                <Rhs vislevel="0" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="b">
                <Rhs vislevel="0" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="super">
                <Rhs vislevel="0" flags="4">
                  <iImage image_left="-1" image_top="-1" image_mag="-1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="-1" offset="-1" falsecolour="false" type="true"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="shim">
                <Rhs vislevel="4" flags="7">
                  <Slider/>
                  <Subcolumn vislevel="2">
                    <Row name="from">
                      <Rhs vislevel="0" flags="4">
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="to">
                      <Rhs vislevel="0" flags="4">
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="value">
                      <Rhs vislevel="0" flags="4">
                        <iText formula="C3"/>
                      </Rhs>
                    </Row>
                    <Row name="super">
                      <Rhs vislevel="1" flags="1">
                        <Subcolumn vislevel="0"/>
                        <iText/>
                        <Real/>
                      </Rhs>
                    </Row>
                    <Row name="caption">
                      <Rhs vislevel="0" flags="4">
                        <iText/>
                      </Rhs>
                    </Row>
                  </Subcolumn>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="align">
                <Rhs vislevel="1" flags="1">
                  <Option/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="bg_colour">
                <Rhs vislevel="1" flags="1">
                  <Expression caption="Background colour"/>
                  <Subcolumn vislevel="0">
                    <Row name="caption">
                      <Rhs vislevel="0" flags="4">
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="expr">
                      <Rhs vislevel="0" flags="4">
                        <iText formula="B8"/>
                      </Rhs>
                    </Row>
                    <Row name="super">
                      <Rhs vislevel="1" flags="1">
                        <Subcolumn vislevel="0"/>
                        <iText/>
                        <Colour/>
                      </Rhs>
                    </Row>
                  </Subcolumn>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Image_join_item.Left_right_item.action C1 C2"/>
          </Rhs>
        </Row>
        <Row popup="false" name="C5">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="-1" image_top="-1" image_mag="-1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="-1" offset="-1" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1">
              <Row name="x">
                <Rhs vislevel="3" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="super">
                <Rhs vislevel="0" flags="4">
                  <iImage image_left="-1" image_top="-1" image_mag="-1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="-1" offset="-1" falsecolour="false" type="true"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="position">
                <Rhs vislevel="1" flags="1">
                  <Option/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="nwidth">
                <Rhs vislevel="1" flags="1">
                  <Expression caption="New width (pixels)"/>
                  <Subcolumn vislevel="0">
                    <Row name="caption">
                      <Rhs vislevel="0" flags="4">
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="expr">
                      <Rhs vislevel="0" flags="4">
                        <iText formula="C4.width + C3 * 2"/>
                      </Rhs>
                    </Row>
                    <Row name="super">
                      <Rhs vislevel="1" flags="4">
                        <Subcolumn vislevel="0"/>
                        <iText/>
                      </Rhs>
                    </Row>
                  </Subcolumn>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="nheight">
                <Rhs vislevel="1" flags="1">
                  <Expression caption="New height (pixels)"/>
                  <Subcolumn vislevel="0">
                    <Row name="caption">
                      <Rhs vislevel="0" flags="4">
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="expr">
                      <Rhs vislevel="0" flags="4">
                        <iText formula="C4.height+ C3 * 2 "/>
                      </Rhs>
                    </Row>
                    <Row name="super">
                      <Rhs vislevel="1" flags="4">
                        <Subcolumn vislevel="0"/>
                        <iText/>
                      </Rhs>
                    </Row>
                  </Subcolumn>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="bgcolour">
                <Rhs vislevel="1" flags="1">
                  <Expression caption="Background colour"/>
                  <Subcolumn vislevel="0">
                    <Row name="caption">
                      <Rhs vislevel="0" flags="4">
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="expr">
                      <Rhs vislevel="0" flags="4">
                        <iText formula="B8"/>
                      </Rhs>
                    </Row>
                    <Row name="super">
                      <Rhs vislevel="1" flags="4">
                        <Subcolumn vislevel="0"/>
                        <iText/>
                      </Rhs>
                    </Row>
                  </Subcolumn>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="left">
                <Rhs vislevel="1" flags="1">
                  <Expression caption="Pixels from left"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="top">
                <Rhs vislevel="1" flags="1">
                  <Expression caption="Pixels from top"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Image_transform_item.Resize_item.Resize_item.Resize_item.Resize_canvas_item.action C4"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="1595" y="0" open="true" selected="false" sform="false" next="10" name="D" caption="make page of cards">
      <Subcolumn vislevel="3">
        <Row popup="false" name="D1">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="-1" image_top="-1" image_mag="-1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="-1" offset="-1" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="C5"/>
          </Rhs>
        </Row>
        <Row popup="false" name="D4">
          <Rhs vislevel="3" flags="7">
            <iImage window_x="0" window_y="0" window_width="650" window_height="650" image_left="909" image_top="294" image_mag="1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1">
              <Row name="default_type">
                <Rhs vislevel="0" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="x">
                <Rhs vislevel="3" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="super">
                <Rhs vislevel="0" flags="4">
                  <iImage image_left="-1" image_top="-1" image_mag="-1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="-1" offset="-1" falsecolour="false" type="true"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="across">
                <Rhs vislevel="1" flags="1">
                  <Expression caption="Tiles across"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="down">
                <Rhs vislevel="1" flags="1">
                  <Expression caption="Tiles down"/>
                  <Subcolumn vislevel="0">
                    <Row name="caption">
                      <Rhs vislevel="0" flags="4">
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="expr">
                      <Rhs vislevel="0" flags="4">
                        <iText formula="5"/>
                      </Rhs>
                    </Row>
                    <Row name="super">
                      <Rhs vislevel="1" flags="4">
                        <Subcolumn vislevel="0"/>
                        <iText/>
                      </Rhs>
                    </Row>
                  </Subcolumn>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="repeat">
                <Rhs vislevel="1" flags="1">
                  <Option caption="Tile type" labelsn="2" labels0="Replicate" labels1="Four-way mirror" value="0"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Image_tile_item.Replicate_item.action D1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="D5">
          <Rhs vislevel="3" flags="7">
            <iImage window_x="0" window_y="6" window_width="650" window_height="650" image_left="309" image_top="2393" image_mag="1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1">
              <Row name="x">
                <Rhs vislevel="3" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="super">
                <Rhs vislevel="0" flags="4">
                  <iImage image_left="-1" image_top="-1" image_mag="-1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="-1" offset="-1" falsecolour="false" type="true"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="l">
                <Rhs vislevel="1" flags="1">
                  <Expression caption="Crop left"/>
                  <Subcolumn vislevel="0">
                    <Row name="caption">
                      <Rhs vislevel="0" flags="4">
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="expr">
                      <Rhs vislevel="0" flags="4">
                        <iText formula="C3"/>
                      </Rhs>
                    </Row>
                    <Row name="super">
                      <Rhs vislevel="1" flags="4">
                        <Subcolumn vislevel="0"/>
                        <iText/>
                      </Rhs>
                    </Row>
                  </Subcolumn>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="t">
                <Rhs vislevel="1" flags="1">
                  <Expression caption="Crop top"/>
                  <Subcolumn vislevel="0">
                    <Row name="caption">
                      <Rhs vislevel="0" flags="4">
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="expr">
                      <Rhs vislevel="0" flags="4">
                        <iText formula="C3"/>
                      </Rhs>
                    </Row>
                    <Row name="super">
                      <Rhs vislevel="1" flags="4">
                        <Subcolumn vislevel="0"/>
                        <iText/>
                      </Rhs>
                    </Row>
                  </Subcolumn>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="w">
                <Rhs vislevel="1" flags="1">
                  <Expression caption="Crop width"/>
                  <Subcolumn vislevel="0">
                    <Row name="caption">
                      <Rhs vislevel="0" flags="4">
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="expr">
                      <Rhs vislevel="0" flags="4">
                        <iText formula="D4.width - C3 * 2"/>
                      </Rhs>
                    </Row>
                    <Row name="super">
                      <Rhs vislevel="1" flags="4">
                        <Subcolumn vislevel="0"/>
                        <iText/>
                      </Rhs>
                    </Row>
                  </Subcolumn>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="h">
                <Rhs vislevel="1" flags="1">
                  <Expression caption="Crop height"/>
                  <Subcolumn vislevel="0">
                    <Row name="caption">
                      <Rhs vislevel="0" flags="4">
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="expr">
                      <Rhs vislevel="0" flags="4">
                        <iText formula="D4.height - C3 * 2"/>
                      </Rhs>
                    </Row>
                    <Row name="super">
                      <Rhs vislevel="1" flags="4">
                        <Subcolumn vislevel="0"/>
                        <iText/>
                      </Rhs>
                    </Row>
                  </Subcolumn>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Image_crop_item.action D4"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
  </Workspace>
</root>



