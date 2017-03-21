<?xml version="1.0"?>
<root xmlns="http://www.vips.ecs.soton.ac.uk/nip/8.5.0">
  <Workspace filename="test_tasks.ws" view="WORKSPACE_MODE_REGULAR" scale="1" offset="0" window_width="1920" window_height="1165" lpane_position="400" lpane_open="false" rpane_position="100" rpane_open="false" local_defs="// private definitions for this workspace&#10;" name="test_tasks" caption="Default empty workspace">
    <Column x="0" y="0" open="true" selected="false" sform="false" next="17" name="B" caption="Colour / Colourspace">
      <Subcolumn vislevel="3">
        <Row popup="false" name="B1">
          <Rhs vislevel="1" flags="1">
            <iImage window_x="0" window_y="30" window_width="512" window_height="729" image_left="229" image_top="309" image_mag="1" show_status="true" show_paintbox="false" show_convert="true" show_rulers="true" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Image_file &quot;$VIPSHOME/share/nip2/data/examples/businesscard/slanted_oval_vase2.jpg&quot;"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="476" y="0" open="true" selected="false" sform="false" next="33" name="BC" caption="Tasks / Capture">
      <Subcolumn vislevel="3">
        <Row popup="false" name="BC2">
          <Rhs vislevel="1" flags="1">
            <iImage window_x="0" window_y="30" window_width="512" window_height="729" image_left="240" image_top="318" image_mag="1" show_status="true" show_paintbox="false" show_convert="false" show_rulers="false" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="B1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="BC3">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Tasks_capture_item.Smooth_image_item.action BC2"/>
          </Rhs>
        </Row>
        <Row popup="false" name="BC4">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Tasks_capture_item.Light_correct_item.action BC2 BC3"/>
          </Rhs>
        </Row>
        <Row popup="false" name="BC5">
          <Rhs vislevel="1" flags="4">
            <iText formula="[BC2,BC3,BC4]"/>
          </Rhs>
        </Row>
        <Row popup="false" name="BC6">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Tasks_capture_item.Image_rank_item.action BC5"/>
          </Rhs>
        </Row>
        <Row popup="false" name="BC7">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1">
              <Row name="x">
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
              <Row name="tilt">
                <Rhs vislevel="1" flags="1">
                  <Slider caption="Left-right tilt" from="-1" to="1" value="-0.40000000000000002"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Tasks_capture_item.Tilt_item.Left_right_item.action B1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="BC8">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1">
              <Row name="x">
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
              <Row name="tilt">
                <Rhs vislevel="1" flags="1">
                  <Slider caption="Top-bottom tilt" from="-1" to="1" value="-0.40000000000000002"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Tasks_capture_item.Tilt_item.Top_bottom_item.action BC7"/>
          </Rhs>
        </Row>
        <Row popup="false" name="BC9">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1">
              <Row name="x">
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
              <Row name="tilt">
                <Rhs vislevel="1" flags="1">
                  <Slider caption="Left-right tilt" from="-1" to="1" value="-0.40000000000000002"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="shift">
                <Rhs vislevel="1" flags="1">
                  <Slider caption="Shift by" from="-1" to="1" value="0.40000000000000002"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Tasks_capture_item.Tilt_item.Left_right_cos_item.action BC8"/>
          </Rhs>
        </Row>
        <Row popup="false" name="BC10">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1">
              <Row name="x">
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
              <Row name="tilt">
                <Rhs vislevel="1" flags="1">
                  <Slider caption="Top-bottom tilt" from="-1" to="1" value="-0.40000000000000002"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="shift">
                <Rhs vislevel="1" flags="1">
                  <Slider/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Tasks_capture_item.Tilt_item.Top_bottom_cos_item.action BC9"/>
          </Rhs>
        </Row>
        <Row popup="false" name="BC11">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1">
              <Row name="x">
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
              <Row name="tilt">
                <Rhs vislevel="1" flags="1">
                  <Slider caption="Tilt" from="-1" to="1" value="0.60000000000000009"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="hshift">
                <Rhs vislevel="1" flags="1">
                  <Slider/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="vshift">
                <Rhs vislevel="1" flags="1">
                  <Slider/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Tasks_capture_item.Tilt_item.Circular_item.action BC10"/>
          </Rhs>
        </Row>
        <Row popup="false" name="BC12">
          <Rhs vislevel="1" flags="1">
            <iRegion image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true">
              <iRegiongroup/>
            </iRegion>
            <Subcolumn vislevel="0"/>
            <iText formula="Region BC2 336 80 35 47"/>
          </Rhs>
        </Row>
        <Row popup="false" name="BC13">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Tasks_capture_item.White_balance_item.action BC11 BC12"/>
          </Rhs>
        </Row>
        <Row popup="false" name="BC14">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="B1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="BC16">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Tasks_capture_item.Tone_item.action BC14"/>
          </Rhs>
        </Row>
        <Row popup="false" name="BC21">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="B1"/>
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
      </Subcolumn>
    </Column>
    <Column x="1100" y="0" open="true" selected="false" sform="false" next="31" name="UB" caption="Tasks / Mosaic / Onepoint|Twopoint">
      <Subcolumn vislevel="3">
        <Row popup="false" name="UB1">
          <Rhs vislevel="1" flags="1">
            <iImage window_x="5" window_y="54" window_width="512" window_height="729" image_left="240" image_top="318" image_mag="1" show_status="true" show_paintbox="false" show_convert="false" show_rulers="false" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="B1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="UB2">
          <Rhs vislevel="1" flags="1">
            <iRegion window_x="193" window_y="129" window_width="613" window_height="909" image_left="299" image_top="417" image_mag="1" show_status="true" show_paintbox="false" show_convert="false" show_rulers="false" scale="1" offset="0" falsecolour="false" type="true" left="27" top="54" width="202" height="453">
              <iRegiongroup/>
            </iRegion>
            <Subcolumn vislevel="0"/>
            <iText formula="Region UB1 50 171 202 228"/>
          </Rhs>
        </Row>
        <Row popup="false" name="UB3">
          <Rhs vislevel="1" flags="1">
            <iRegion window_x="848" window_y="305" window_width="613" window_height="284" image_left="72" image_top="28" image_mag="4" show_status="true" show_paintbox="false" show_convert="false" show_rulers="false" scale="1" offset="0" falsecolour="false" type="true" left="180" top="75" width="180" height="448">
              <iRegiongroup/>
            </iRegion>
            <Subcolumn vislevel="0"/>
            <iText formula="Region UB1 193 248 180 243"/>
          </Rhs>
        </Row>
        <Row popup="false" name="UB4">
          <Rhs vislevel="1" flags="1">
            <iArrow>
              <iRegiongroup/>
            </iArrow>
            <Subcolumn vislevel="0"/>
            <iText formula="Mark UB2 182 53"/>
          </Rhs>
        </Row>
        <Row popup="false" name="UB5">
          <Rhs vislevel="1" flags="1">
            <iArrow left="30" top="31" width="0" height="0">
              <iRegiongroup/>
            </iArrow>
            <Subcolumn vislevel="0"/>
            <iText formula="Mark UB3 11 22"/>
          </Rhs>
        </Row>
        <Row popup="false" name="UB8">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Tasks_mosaic_item.Mosaic_1point_item.Left_right_item.action UB4 UB5"/>
          </Rhs>
        </Row>
        <Row popup="false" name="UB10">
          <Rhs vislevel="1" flags="1">
            <iRegion window_x="0" window_y="30" window_width="613" window_height="237" image_left="290" image_top="81" image_mag="1" show_status="true" show_paintbox="false" show_convert="false" show_rulers="false" scale="1" offset="0" falsecolour="false" type="true">
              <iRegiongroup/>
            </iRegion>
            <Subcolumn vislevel="0"/>
            <iText formula="Region UB1 52 189 372 196"/>
          </Rhs>
        </Row>
        <Row popup="false" name="UB12">
          <Rhs vislevel="1" flags="1">
            <iRegion window_x="5" window_y="54" window_width="613" window_height="249" image_left="290" image_top="87" image_mag="1" show_status="true" show_paintbox="false" show_convert="false" show_rulers="false" scale="1" offset="0" falsecolour="false" type="true">
              <iRegiongroup/>
            </iRegion>
            <Subcolumn vislevel="0"/>
            <iText formula="Region UB1 12 250 433 208"/>
          </Rhs>
        </Row>
        <Row popup="false" name="UB13">
          <Rhs vislevel="1" flags="1">
            <iArrow>
              <iRegiongroup/>
            </iArrow>
            <Subcolumn vislevel="0"/>
            <iText formula="Mark UB10 318 104"/>
          </Rhs>
        </Row>
        <Row popup="false" name="UB14">
          <Rhs vislevel="1" flags="1">
            <iArrow>
              <iRegiongroup/>
            </iArrow>
            <Subcolumn vislevel="0"/>
            <iText formula="Mark UB12 362 47"/>
          </Rhs>
        </Row>
        <Row popup="false" name="UB15">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Tasks_mosaic_item.Mosaic_1point_item.Top_bottom_item.action UB13 UB14"/>
          </Rhs>
        </Row>
        <Row popup="false" name="UB16">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Tasks_mosaic_item.Mosaic_1point_item.Left_right_manual_item.action UB4 UB5"/>
          </Rhs>
        </Row>
        <Row popup="false" name="UB17">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Tasks_mosaic_item.Mosaic_1point_item.Top_bottom_manual_item.action UB13 UB14"/>
          </Rhs>
        </Row>
        <Row popup="false" name="UB18">
          <Rhs vislevel="1" flags="1">
            <iImage window_x="10" window_y="78" window_width="569" window_height="750" image_left="268" image_top="328" image_mag="1" show_status="true" show_paintbox="false" show_convert="false" show_rulers="false" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="BC22"/>
          </Rhs>
        </Row>
        <Row popup="false" name="UB19">
          <Rhs vislevel="1" flags="1">
            <iRegion window_x="463" window_y="317" window_width="446" window_height="260" image_left="378" image_top="20" image_mag="4" show_status="true" show_paintbox="false" show_convert="false" show_rulers="false" scale="1" offset="0" falsecolour="false" type="true">
              <iRegiongroup/>
            </iRegion>
            <Subcolumn vislevel="0"/>
            <iText formula="Region UB18 26 289 430 219"/>
          </Rhs>
        </Row>
        <Row popup="false" name="UB20">
          <Rhs vislevel="1" flags="1">
            <iArrow>
              <iRegiongroup/>
            </iArrow>
            <Subcolumn vislevel="0"/>
            <iText formula="Mark UB19 378 40"/>
          </Rhs>
        </Row>
        <Row popup="false" name="UB21">
          <Rhs vislevel="1" flags="1">
            <iArrow left="139" top="42" width="0" height="0">
              <iRegiongroup/>
            </iArrow>
            <Subcolumn vislevel="0"/>
            <iText formula="Mark UB19 150 23"/>
          </Rhs>
        </Row>
        <Row popup="false" name="UB22">
          <Rhs vislevel="1" flags="1">
            <iArrow>
              <iRegiongroup/>
            </iArrow>
            <Subcolumn vislevel="0"/>
            <iText formula="Mark UB10 81 131"/>
          </Rhs>
        </Row>
        <Row popup="false" name="UB23">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Tasks_mosaic_item.Mosaic_2point_item.Top_bottom_item.action UB13 UB20 UB21 UB22"/>
          </Rhs>
        </Row>
        <Row popup="false" name="UB24">
          <Rhs vislevel="1" flags="1">
            <iRegion window_x="849" window_y="110" window_width="374" window_height="497" image_left="171" image_top="211" image_mag="1" show_status="true" show_paintbox="false" show_convert="false" show_rulers="false" scale="1" offset="0" falsecolour="false" type="true">
              <iRegiongroup/>
            </iRegion>
            <Subcolumn vislevel="0"/>
            <iText formula="Region UB18 213 78 158 456"/>
          </Rhs>
        </Row>
        <Row popup="false" name="UB25">
          <Rhs vislevel="1" flags="1">
            <iArrow>
              <iRegiongroup/>
            </iArrow>
            <Subcolumn vislevel="0"/>
            <iText formula="Mark UB24 45 42"/>
          </Rhs>
        </Row>
        <Row popup="false" name="UB26">
          <Rhs vislevel="1" flags="1">
            <iArrow>
              <iRegiongroup/>
            </iArrow>
            <Subcolumn vislevel="0"/>
            <iText formula="Mark UB24 8 402"/>
          </Rhs>
        </Row>
        <Row popup="false" name="UB27">
          <Rhs vislevel="1" flags="1">
            <iArrow>
              <iRegiongroup/>
            </iArrow>
            <Subcolumn vislevel="0"/>
            <iText formula="Mark UB2 175 410"/>
          </Rhs>
        </Row>
        <Row popup="false" name="UB28">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Tasks_mosaic_item.Mosaic_2point_item.Left_right_item.action UB4 UB27 UB26 UB25"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="1733" y="0" open="true" selected="false" sform="false" next="12" name="IB" caption="Tasks / Mosaic / Manual Balance">
      <Subcolumn vislevel="3">
        <Row popup="false" name="IB8">
          <Rhs vislevel="1" flags="4">
            <iText formula="&quot;$VIPSHOME/share/nip2/data/examples/manual_balance/&quot;"/>
          </Rhs>
        </Row>
        <Row popup="false" name="IB1">
          <Rhs vislevel="1" flags="1">
            <iImage window_x="602" window_y="480" window_width="745" window_height="689" image_left="364" image_top="276" image_mag="1" show_status="true" show_paintbox="false" show_convert="true" show_rulers="false" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Image_file (IB8 ++ &quot;simp_base.png&quot;)"/>
          </Rhs>
        </Row>
        <Row popup="false" name="IB2">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="-1" image_top="-1" image_mag="-1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="-1" offset="-1" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Image_file (IB8 ++ &quot;mask_control.png&quot;)"/>
          </Rhs>
        </Row>
        <Row popup="false" name="IB3">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="-1" image_top="-1" image_mag="-1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="-1" offset="-1" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Image_file (IB8 ++ &quot;mask_01.png&quot;)"/>
          </Rhs>
        </Row>
        <Row popup="false" name="IB4">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="-1" image_top="-1" image_mag="-1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="-1" offset="-1" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Image_file (IB8 ++ &quot;mask_02.png&quot;)"/>
          </Rhs>
        </Row>
        <Row popup="false" name="IB5">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="-1" image_top="-1" image_mag="-1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="-1" offset="-1" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Image_file (IB8 ++ &quot;mask_03.png&quot;)"/>
          </Rhs>
        </Row>
        <Row popup="false" name="IB6">
          <Rhs vislevel="1" flags="1">
            <Group/>
            <Subcolumn vislevel="0"/>
            <iText formula="Group [IB3,IB4,IB5]"/>
          </Rhs>
        </Row>
        <Row popup="false" name="IB9">
          <Rhs vislevel="1" flags="1">
            <Matrix/>
            <Subcolumn vislevel="0"/>
            <iText formula="Tasks_mosaic_item.Manual_balance_item.Balance_find_item.action IB1 IB2 IB6"/>
          </Rhs>
        </Row>
        <Row popup="false" name="IB10">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="-1" image_top="-1" image_mag="-1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="-1" offset="-1" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1">
              <Row name="im_in">
                <Rhs vislevel="0" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="m_matrix">
                <Rhs vislevel="1" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="m_group">
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
              <Row name="blur">
                <Rhs vislevel="1" flags="1">
                  <Slider/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="adjust">
                <Rhs vislevel="1" flags="1">
                  <Matrix/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="Build">
                <Rhs vislevel="1" flags="1">
                  <Toggle caption="Build Scale and Offset Correction Images" value="true"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="Output">
                <Rhs vislevel="2" flags="6">
                  <Subcolumn vislevel="1"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="value">
                <Rhs vislevel="1" flags="4">
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Tasks_mosaic_item.Manual_balance_item.Balance_check_item.action IB1 IB9 IB6"/>
          </Rhs>
        </Row>
        <Row popup="false" name="IB11">
          <Rhs vislevel="2" flags="5">
            <iImage window_x="2997" window_y="620" window_width="516" window_height="543" image_left="240" image_top="240" image_mag="1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="-1" offset="-1" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Tasks_mosaic_item.Manual_balance_item.Balance_apply_item.action IB1 IB10.Output.scale_im IB10.Output.offset_im"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="2568" y="0" open="true" selected="false" sform="false" next="6" name="EC" caption="Tasks / Mosaic / Clone">
      <Subcolumn vislevel="3">
        <Row popup="false" name="EC4">
          <Rhs vislevel="1" flags="4">
            <iText formula="&quot;$VIPSHOME/share/nip2/data/examples/clone/&quot;"/>
          </Rhs>
        </Row>
        <Row popup="false" name="EC1">
          <Rhs vislevel="2" flags="5">
            <iImage window_x="225" window_y="466" window_width="1108" window_height="532" image_left="305" image_top="70" image_mag="4" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="-1" offset="-1" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Image_file (EC4 ++ &quot;example_im_01.png&quot;)"/>
          </Rhs>
        </Row>
        <Row popup="false" name="EC2">
          <Rhs vislevel="2" flags="5">
            <iImage window_x="0" window_y="0" window_width="478" window_height="526" image_left="58" image_top="57" image_mag="4" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="-1" offset="-1" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Image_file (EC4 ++ &quot;example_im_02.png&quot;)"/>
          </Rhs>
        </Row>
        <Row popup="false" name="EC5">
          <Rhs vislevel="3" flags="7">
            <iImage window_x="0" window_y="0" window_width="639" window_height="532" image_left="364" image_top="90" image_mag="4" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="-1" offset="-1" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1">
              <Row name="im1">
                <Rhs vislevel="0" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="im2">
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
              <Row name="r1">
                <Rhs vislevel="1" flags="1">
                  <iRegion image_left="-1" image_top="-1" image_mag="-1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="-1" offset="-1" falsecolour="false" type="true" left="406" top="49" width="28" height="37">
                    <iRegiongroup/>
                  </iRegion>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="p2">
                <Rhs vislevel="1" flags="1">
                  <iArrow left="86" top="23" width="0" height="0">
                    <iRegiongroup/>
                  </iArrow>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="mask">
                <Rhs vislevel="1" flags="1">
                  <iImage image_left="-1" image_top="-1" image_mag="-1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="-1" offset="-1" falsecolour="false" type="true"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="Options">
                <Rhs vislevel="3" flags="6">
                  <Subcolumn vislevel="2">
                    <Row name="super">
                      <Rhs vislevel="0" flags="4">
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="pause">
                      <Rhs vislevel="1" flags="1">
                        <Toggle caption="Pause process" value="true"/>
                        <Subcolumn vislevel="0"/>
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="replace">
                      <Rhs vislevel="1" flags="1">
                        <Option/>
                        <Subcolumn vislevel="0"/>
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="balance">
                      <Rhs vislevel="1" flags="1">
                        <Toggle/>
                        <Subcolumn vislevel="0"/>
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="process">
                      <Rhs vislevel="1" flags="1">
                        <Toggle caption="Replace area with Gaussian noise." value="false"/>
                        <Subcolumn vislevel="0"/>
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="sc">
                      <Rhs vislevel="1" flags="1">
                        <Slider/>
                        <Subcolumn vislevel="0"/>
                        <iText/>
                      </Rhs>
                    </Row>
                  </Subcolumn>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Tasks_mosaic_item.Clone_area_item.action EC1 EC2"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
  </Workspace>
</root>



