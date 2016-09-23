<?xml version="1.0"?>
<root xmlns="http://www.vips.ecs.soton.ac.uk/nip/8.4.0">
  <Workspace window_x="0" window_y="0" window_width="1115" window_height="800" filename="$HOME/GIT/nip2/share/nip2/data/examples/overlays_and_blending/overlay_blend.ws" view="WORKSPACE_MODE_REGULAR" scale="1" offset="0" lpane_position="400" lpane_open="false" rpane_position="100" rpane_open="false" local_defs="// private definitions for this workspace&#10;" name="overlay_blend" caption="Default empty workspace">
    <Column x="0" y="0" open="true" selected="false" sform="false" next="5" name="A" caption="Images">
      <Subcolumn vislevel="3">
        <Row popup="false" name="A1">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="-1" image_top="-1" image_mag="-1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="-1" offset="-1" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Image_file &quot;$HOME/GIT/nip2/share/nip2/data/examples/overlays_and_blending/blend_example_ir.jpg&quot;"/>
          </Rhs>
        </Row>
        <Row popup="false" name="A2">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="-1" image_top="-1" image_mag="-1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="-1" offset="-1" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Image_file &quot;$HOME/GIT/nip2/share/nip2/data/examples/overlays_and_blending/blend_example_vis.jpg&quot;"/>
          </Rhs>
        </Row>
        <Row popup="false" name="A3">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="-1" image_top="-1" image_mag="-1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="-1" offset="-1" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Image_file &quot;$HOME/GIT/nip2/share/nip2/data/examples/overlays_and_blending/blend_example_xray.jpg&quot;"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="466" y="0" open="true" selected="false" sform="false" next="5" name="B" caption="Complete Transparency">
      <Subcolumn vislevel="3">
        <Row popup="false" name="B1">
          <Rhs vislevel="3" flags="7">
            <iText formula="Filter_blend_item.Scale_blend_item.action A2 A1"/>
            <iImage image_left="-1" image_top="-1" image_mag="-1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="-1" offset="-1" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="466" y="208" open="true" selected="true" sform="false" next="3" name="C" caption="Blending Between two, then three Images">
      <Subcolumn vislevel="3">
        <Row popup="false" name="C1">
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
              <Row name="orientation">
                <Rhs vislevel="1" flags="1">
                  <Option/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="blend_position">
                <Rhs vislevel="1" flags="1">
                  <Slider caption="Blend position" from="0" to="1" value="0.28767123287671231"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="blend_width">
                <Rhs vislevel="1" flags="1">
                  <Slider caption="Blend width" from="0" to="1" value="0.20547945205479454"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Filter_blend_item.Line_blend_item.action A1 A2"/>
          </Rhs>
        </Row>
        <Row popup="false" name="C2">
          <Rhs vislevel="3" flags="7">
            <iImage window_x="1600" window_y="652" window_width="472" window_height="511" image_left="218" image_top="207" image_mag="1" show_status="false" show_paintbox="false" show_convert="true" show_rulers="false" scale="1" offset="0" falsecolour="false" type="true"/>
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
                  <iImage image_left="-1" image_top="-1" image_mag="-1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="-1" offset="-1" falsecolour="false" type="true"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="orientation">
                <Rhs vislevel="1" flags="1">
                  <Option/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="blend_position">
                <Rhs vislevel="1" flags="1">
                  <Slider caption="Blend position" from="0" to="1" value="0.71232876712328774"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="blend_width">
                <Rhs vislevel="1" flags="1">
                  <Slider/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Filter_blend_item.Line_blend_item.action C1 A3"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="945" y="0" open="true" selected="false" sform="false" next="2" name="D" caption="Overlay">
      <Subcolumn vislevel="3">
        <Row popup="false" name="D1">
          <Rhs vislevel="3" flags="7">
            <iImage window_x="2554" window_y="136" window_width="619" window_height="590" image_left="301" image_top="256" image_mag="1" show_status="false" show_paintbox="false" show_convert="true" show_rulers="false" scale="1" offset="0" falsecolour="false" type="true"/>
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
              <Row name="colour">
                <Rhs vislevel="1" flags="1">
                  <Option caption="Colour overlay as" labelsn="6" labels0="Green over Red" labels1="Blue over Red" labels2="Red over Green" labels3="Red over Blue" labels4="Blue over Green" labels5="Green over Blue" value="0"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Filter_overlay_header_item.action A2 A1"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
  </Workspace>
</root>



