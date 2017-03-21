<?xml version="1.0"?>
<root xmlns="http://www.vips.ecs.soton.ac.uk/nip/8.5.0">
  <Workspace filename="test_transform.ws" view="WORKSPACE_MODE_REGULAR" scale="1" offset="0" window_width="1279" window_height="705" lpane_position="0" lpane_open="false" rpane_position="1914" rpane_open="false" local_defs="// private definitions for this workspace&#10;" name="test_transform" caption="Default empty workspace">
    <Column x="476" y="0" open="true" selected="false" sform="false" next="19" name="FB" caption="Image / Transform">
      <Subcolumn vislevel="3">
        <Row popup="false" name="FB1">
          <Rhs vislevel="0" flags="4">
            <iImage window_x="5" window_y="54" window_width="512" window_height="729" image_left="476" image_top="596" image_mag="-2" show_status="true" show_paintbox="false" show_convert="true" show_rulers="true" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="A1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="FB11">
          <Rhs vislevel="3" flags="7">
            <iImage window_x="5" window_y="54" window_width="512" window_height="729" image_left="229" image_top="355" image_mag="1" show_status="true" show_paintbox="false" show_convert="true" show_rulers="true" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1">
              <Row name="x">
                <Rhs vislevel="0" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="super">
                <Rhs vislevel="0" flags="4">
                  <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="dir">
                <Rhs vislevel="1" flags="1">
                  <Option caption="Select distort direction" labelsn="2" labels0="Distort to points" labels1="Distort to corners" value="0"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="ap1">
                <Rhs vislevel="1" flags="1">
                  <iArrow>
                    <iRegiongroup/>
                  </iArrow>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="ap2">
                <Rhs vislevel="1" flags="1">
                  <iArrow left="480" top="152" width="0" height="0">
                    <iRegiongroup/>
                  </iArrow>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="ap3">
                <Rhs vislevel="1" flags="1">
                  <iArrow left="474" top="504" width="0" height="0">
                    <iRegiongroup/>
                  </iArrow>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="ap4">
                <Rhs vislevel="1" flags="1">
                  <iArrow left="20" top="654" width="0" height="0">
                    <iRegiongroup/>
                  </iArrow>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Image_transform_item.Image_perspective_item.action FB1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="FB13">
          <Rhs vislevel="3" flags="7">
            <Matrix/>
            <Subcolumn vislevel="1"/>
            <iText formula="Image_transform_item.Image_rubber_item.Find_item.action FB11 FB1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="FB14">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Image_transform_item.Image_rubber_item.Apply_item.action FB11 FB13"/>
          </Rhs>
        </Row>
        <Row popup="false" name="FB16">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Image_transform_item.Image_perspective_match_item.action FB11 FB1"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="1179" y="0" open="true" selected="false" sform="false" next="33" name="BC" caption="Tasks / Capture">
      <Subcolumn vislevel="3">
        <Row popup="false" name="BC21">
          <Rhs vislevel="2" flags="5">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="A1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="BC22">
          <Rhs vislevel="3" flags="7">
            <iImage window_x="0" window_y="30" window_width="625" window_height="750" image_left="296" image_top="432" image_mag="1" show_status="true" show_paintbox="false" show_convert="false" show_rulers="false" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1">
              <Row name="x">
                <Rhs vislevel="0" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="super">
                <Rhs vislevel="0" flags="4">
                  <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="angle">
                <Rhs vislevel="1" flags="1">
                  <Slider caption="Angle" from="-180" to="180" value="-3.8297872340425556"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Tasks_capture_item.Rotate_item.Free_item.action BC21"/>
          </Rhs>
        </Row>
        <Row popup="false" name="BC30">
          <Rhs vislevel="2" flags="5">
            <iRegion image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true">
              <iRegiongroup/>
            </iRegion>
            <Subcolumn vislevel="0"/>
            <iText formula="Region BC22 0 0 496 688"/>
          </Rhs>
        </Row>
        <Row popup="false" name="BC31">
          <Rhs vislevel="3" flags="7">
            <Matrix/>
            <Subcolumn vislevel="1">
              <Row name="reference">
                <Rhs vislevel="0" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="sample">
                <Rhs vislevel="0" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="super">
                <Rhs vislevel="0" flags="4">
                  <Matrix/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="order">
                <Rhs vislevel="1" flags="1">
                  <Option/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="interp">
                <Rhs vislevel="1" flags="1">
                  <Option/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="wrap">
                <Rhs vislevel="1" flags="1">
                  <Toggle/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="max_err">
                <Rhs vislevel="1" flags="1">
                  <Expression caption="Maximum error"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="max_iter">
                <Rhs vislevel="1" flags="1">
                  <Expression caption="Maximum iterations"/>
                  <Subcolumn vislevel="0">
                    <Row name="caption">
                      <Rhs vislevel="0" flags="4">
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="expr">
                      <Rhs vislevel="0" flags="4">
                        <iText formula="100"/>
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
              <Row name="transformed_image">
                <Rhs vislevel="1" flags="1">
                  <iImage window_x="5" window_y="54" window_width="512" window_height="729" image_left="240" image_top="318" image_mag="1" show_status="true" show_paintbox="false" show_convert="false" show_rulers="false" scale="1" offset="0" falsecolour="false" type="true"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="final_error">
                <Rhs vislevel="1" flags="4">
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Tasks_capture_item.Rubber_item.Find_item.action BC21 BC30"/>
          </Rhs>
        </Row>
        <Row popup="false" name="BC32">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1">
              <Row name="a">
                <Rhs vislevel="0" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="b">
                <Rhs vislevel="3" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="super">
                <Rhs vislevel="0" flags="4">
                  <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="interp">
                <Rhs vislevel="1" flags="1">
                  <Option/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="wrap">
                <Rhs vislevel="1" flags="1">
                  <Toggle caption="Wrap image edges" value="true"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Tasks_capture_item.Rubber_item.Apply_item.action BC30 BC31"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="1803" y="0" open="true" selected="false" sform="false" next="6" name="GC" caption="untitled">
      <Subcolumn vislevel="3">
        <Row popup="false" name="GC5">
          <Rhs vislevel="1" flags="4">
            <iText formula="&quot;$VIPSHOME/share/nip2/data/examples/framing/&quot;"/>
          </Rhs>
        </Row>
        <Row popup="false" name="GC1">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="-1" image_top="-1" image_mag="-1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="-1" offset="-1" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Image_file (GC5 ++ &quot;framing_picture.jpg&quot;)"/>
          </Rhs>
        </Row>
        <Row popup="false" name="GC2">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="-1" image_top="-1" image_mag="-1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="-1" offset="-1" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Image_file (GC5 ++ &quot;framing_complex.png&quot;)"/>
          </Rhs>
        </Row>
        <Row popup="false" name="GC3">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="-1" image_top="-1" image_mag="-1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="-1" offset="-1" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Image_file (GC5 ++ &quot;framing_corner.png&quot;)"/>
          </Rhs>
        </Row>
        <Row popup="false" name="GC4">
          <Rhs vislevel="1" flags="1">
            <iImage window_x="2172" window_y="197" window_width="664" window_height="802" image_left="39" image_top="446" image_mag="8" show_status="false" show_paintbox="false" show_convert="true" show_rulers="false" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Image_file (GC5 ++ &quot;framing_distorted_frame.png&quot;)"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="1803" y="411" open="true" selected="false" sform="false" next="2" name="HC" caption="Straighten Frame">
      <Subcolumn vislevel="3">
        <Row popup="false" name="HC1">
          <Rhs vislevel="3" flags="7">
            <iImage window_x="1710" window_y="115" window_width="710" window_height="794" image_left="347" image_top="358" image_mag="1" show_status="false" show_paintbox="false" show_convert="true" show_rulers="false" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1">
              <Row name="super">
                <Rhs vislevel="0" flags="4">
                  <iImage image_left="-1" image_top="-1" image_mag="-1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="-1" offset="-1" falsecolour="false" type="true"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="dir">
                <Rhs vislevel="1" flags="1">
                  <Option/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="ap1">
                <Rhs vislevel="1" flags="1">
                  <iArrow left="23" top="43" width="0" height="0">
                    <iRegiongroup/>
                  </iArrow>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="ap2">
                <Rhs vislevel="1" flags="1">
                  <iArrow left="369" top="8" width="0" height="0">
                    <iRegiongroup/>
                  </iArrow>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="ap3">
                <Rhs vislevel="1" flags="1">
                  <iArrow left="388" top="468" width="0" height="0">
                    <iRegiongroup/>
                  </iArrow>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="ap4">
                <Rhs vislevel="1" flags="1">
                  <iArrow left="9" top="485" width="0" height="0">
                    <iRegiongroup/>
                  </iArrow>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="x">
                <Rhs vislevel="0" flags="4">
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Tasks_frame_item.Straighten_frame_item.action GC4"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="2323" y="0" open="true" selected="false" sform="false" next="2" name="IC" caption="Painting with Simple Frame">
      <Subcolumn vislevel="3">
        <Row popup="false" name="IC1">
          <Rhs vislevel="3" flags="7">
            <iImage window_x="1708" window_y="57" window_width="521" window_height="430" image_left="252" image_top="176" image_mag="1" show_status="false" show_paintbox="false" show_convert="true" show_rulers="false" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1">
              <Row name="a">
                <Rhs vislevel="3" flags="4">
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
              <Row name="ppcm">
                <Rhs vislevel="1" flags="1">
                  <Expression caption="Number of pixels per cm"/>
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
              <Row name="overlap">
                <Rhs vislevel="1" flags="1">
                  <Expression caption="Size of frame overlap in cm"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="variables">
                <Rhs vislevel="1" flags="4">
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="mount_options">
                <Rhs vislevel="1" flags="4">
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="frame">
                <Rhs vislevel="1" flags="1">
                  <iImage image_left="-1" image_top="-1" image_mag="-1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="-1" offset="-1" falsecolour="false" type="true"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Tasks_frame_item.Build_frame_item.Simple_frame_item.action HC1 GC1"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="2323" y="387" open="true" selected="false" sform="false" next="2" name="JC" caption="Painting with Complex Frame, with adjusted variables">
      <Subcolumn vislevel="3">
        <Row popup="false" name="JC1">
          <Rhs vislevel="3" flags="7">
            <iImage window_x="1673" window_y="48" window_width="548" window_height="445" image_left="266" image_top="183" image_mag="1" show_status="false" show_paintbox="false" show_convert="true" show_rulers="false" scale="1" offset="0" falsecolour="false" type="true"/>
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
              <Row name="ppcm">
                <Rhs vislevel="1" flags="1">
                  <Expression caption="Number of pixels per cm"/>
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
              <Row name="overlap">
                <Rhs vislevel="1" flags="1">
                  <Expression caption="Size of frame overlap in cm"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="variables">
                <Rhs vislevel="2" flags="6">
                  <Subcolumn vislevel="1">
                    <Row name="super">
                      <Rhs vislevel="0" flags="4">
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="scale_factor">
                      <Rhs vislevel="1" flags="1">
                        <Expression caption="scale the size of the frame by"/>
                        <Subcolumn vislevel="0"/>
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="corner_section">
                      <Rhs vislevel="1" flags="1">
                        <Slider caption="Corner section" from="0.10000000000000001" to="1" value="0.5"/>
                        <Subcolumn vislevel="0"/>
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="edge_section">
                      <Rhs vislevel="1" flags="1">
                        <Slider/>
                        <Subcolumn vislevel="0"/>
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="middle_section">
                      <Rhs vislevel="1" flags="1">
                        <Slider/>
                        <Subcolumn vislevel="0"/>
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="blend_fraction">
                      <Rhs vislevel="1" flags="1">
                        <Slider caption="Blend fraction" from="0.10000000000000001" to="0.90000000000000002" value="0.10000000000000001"/>
                        <Subcolumn vislevel="0"/>
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="option">
                      <Rhs vislevel="1" flags="1">
                        <Toggle/>
                        <Subcolumn vislevel="0"/>
                        <iText/>
                      </Rhs>
                    </Row>
                  </Subcolumn>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="mount_options">
                <Rhs vislevel="1" flags="4">
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="frame">
                <Rhs vislevel="1" flags="1">
                  <iImage image_left="-1" image_top="-1" image_mag="-1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="-1" offset="-1" falsecolour="false" type="true"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Tasks_frame_item.Build_frame_item.Complex_frame_item.action GC2 GC1"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="3002" y="0" open="true" selected="false" sform="false" next="2" name="KC" caption="Painting with frame corner, and coloured mount">
      <Subcolumn vislevel="3">
        <Row popup="false" name="KC1">
          <Rhs vislevel="3" flags="7">
            <iImage window_x="1669" window_y="53" window_width="640" window_height="566" image_left="312" image_top="244" image_mag="1" show_status="false" show_paintbox="false" show_convert="true" show_rulers="false" scale="1" offset="0" falsecolour="false" type="true"/>
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
              <Row name="ppcm">
                <Rhs vislevel="1" flags="1">
                  <Expression caption="Number of pixels per cm"/>
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
              <Row name="overlap">
                <Rhs vislevel="1" flags="1">
                  <Expression caption="Size of frame overlap in cm"/>
                  <Subcolumn vislevel="0">
                    <Row name="caption">
                      <Rhs vislevel="0" flags="4">
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="expr">
                      <Rhs vislevel="0" flags="4">
                        <iText formula="-10"/>
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
              <Row name="variables">
                <Rhs vislevel="1" flags="4">
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="mount_options">
                <Rhs vislevel="2" flags="6">
                  <Subcolumn vislevel="1">
                    <Row name="super">
                      <Rhs vislevel="0" flags="4">
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="apply">
                      <Rhs vislevel="1" flags="1">
                        <Toggle caption="Apply mount options" value="false"/>
                        <Subcolumn vislevel="0"/>
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="ls">
                      <Rhs vislevel="1" flags="1">
                        <Expression caption="Lower mount section bigger by (cm)"/>
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
                    <Row name="mount_colour">
                      <Rhs vislevel="3" flags="7">
                        <Colour colour_space="sRGB" value0="0" value1="0" value2="0"/>
                        <Subcolumn vislevel="1"/>
                        <iText/>
                      </Rhs>
                    </Row>
                  </Subcolumn>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="frame">
                <Rhs vislevel="1" flags="1">
                  <iImage image_left="-1" image_top="-1" image_mag="-1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="-1" offset="-1" falsecolour="false" type="true"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Tasks_frame_item.Build_frame_item.Frame_corner_item.action GC3 GC1"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="0" y="0" open="true" selected="true" sform="false" next="2" name="A" caption="">
      <Subcolumn vislevel="3">
        <Row popup="false" name="A1">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Image_file &quot;$VIPSHOME/share/nip2/data/examples/businesscard/slanted_oval_vase2.jpg&quot;"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
  </Workspace>
</root>



