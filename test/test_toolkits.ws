<?xml version="1.0"?>
<root xmlns="http://www.vips.ecs.soton.ac.uk/nip/7.14.19">
  <Workspace filename="test/test_toolkits.ws" view="WORKSPACE_MODE_REGULAR" scale="1" offset="0" window_width="1920" window_height="1165" lpane_position="400" lpane_open="false" rpane_position="100" rpane_open="false" local_defs="// private definitions for this workspace&#10;" name="test_toolkits" caption="Default empty workspace">
    <Column x="0" y="0" open="true" selected="false" sform="false" next="10" name="A" caption="Colour / New">
      <Subcolumn vislevel="3">
        <Row popup="false" name="A1">
          <Rhs vislevel="3" flags="7">
            <Colour/>
            <Subcolumn vislevel="1"/>
            <iText formula="Colour_new_item.Widget_colour_item.action"/>
          </Rhs>
        </Row>
        <Row popup="false" name="A2">
          <Rhs vislevel="3" flags="7">
            <Colour/>
            <Subcolumn vislevel="1"/>
            <iText formula="Colour_new_item.LAB_colour.action"/>
          </Rhs>
        </Row>
        <Row popup="false" name="A4">
          <Rhs vislevel="1" flags="4">
            <iText formula="if A1.value != A2.value then error &quot;fail&quot; else &quot;ok!&quot;"/>
          </Rhs>
        </Row>
        <Row popup="false" name="A8">
          <Rhs vislevel="1" flags="4">
            <iText formula="12"/>
          </Rhs>
        </Row>
        <Row popup="false" name="A9">
          <Rhs vislevel="1" flags="1">
            <Colour/>
            <Subcolumn vislevel="0"/>
            <iText formula="Colour_to_colour_item.action A8"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="0" y="1000" open="true" selected="false" sform="false" next="17" name="B" caption="Colour / Colourspace">
      <Subcolumn vislevel="3">
        <Row popup="false" name="B1">
          <Rhs vislevel="0" flags="4">
            <iImage window_x="0" window_y="30" window_width="512" window_height="729" image_left="229" image_top="309" image_mag="1" show_status="true" show_paintbox="false" show_convert="true" show_rulers="true" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="FC1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="B2">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Colour_convert_item.Lab_item.action B1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="B3">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Colour_convert_item.LabQ_item.action B2"/>
          </Rhs>
        </Row>
        <Row popup="false" name="B4">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Colour_convert_item.LabS_item.action B3"/>
          </Rhs>
        </Row>
        <Row popup="false" name="B5">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Colour_convert_item.LCh_item.action B4"/>
          </Rhs>
        </Row>
        <Row popup="false" name="B6">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Colour_convert_item.XYZ_item.action B5"/>
          </Rhs>
        </Row>
        <Row popup="false" name="B7">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Colour_convert_item.Yxy_item.action B6"/>
          </Rhs>
        </Row>
        <Row popup="false" name="B8">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Colour_convert_item.UCS_item.action B7"/>
          </Rhs>
        </Row>
        <Row popup="false" name="B9">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Colour_convert_item.sRGB_item.action B8"/>
          </Rhs>
        </Row>
        <Row popup="false" name="B15">
          <Rhs vislevel="1" flags="4">
            <iText formula="&quot;test max difference &lt; 1&quot;"/>
          </Rhs>
        </Row>
        <Row popup="false" name="B12">
          <Rhs vislevel="0" flags="4">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Colour_dE_item.CIEdE76_item.action B1 B9"/>
          </Rhs>
        </Row>
        <Row popup="false" name="B13">
          <Rhs vislevel="1" flags="4">
            <iText formula="Math_stats_item.Max_item.action B12"/>
          </Rhs>
        </Row>
        <Row popup="false" name="B14">
          <Rhs vislevel="1" flags="4">
            <iText formula="if B13 &gt; 1 then error &quot;fail&quot; else &quot;ok!&quot;"/>
          </Rhs>
        </Row>
        <Row popup="false" name="B10">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Colour_convert_item.Mono_item.action B9"/>
          </Rhs>
        </Row>
        <Row popup="false" name="B16">
          <Rhs vislevel="1" flags="4">
            <iText formula="if B10.bands != 1 then error &quot;fail&quot; else &quot;ok!&quot;"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="0" y="2542" open="true" selected="false" sform="false" next="12" name="C" caption="Colour / Tag As">
      <Subcolumn vislevel="3">
        <Row popup="false" name="C1">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="B1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="C2">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Colour_tag_item.Lab_item.action C1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="C3">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Colour_tag_item.LabQ_item.action C2"/>
          </Rhs>
        </Row>
        <Row popup="false" name="C4">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Colour_tag_item.LabS_item.action C3"/>
          </Rhs>
        </Row>
        <Row popup="false" name="C5">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Colour_tag_item.LCh_item.action C4"/>
          </Rhs>
        </Row>
        <Row popup="false" name="C6">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Colour_tag_item.XYZ_item.action C5"/>
          </Rhs>
        </Row>
        <Row popup="false" name="C7">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Colour_tag_item.Yxy_item.action C6"/>
          </Rhs>
        </Row>
        <Row popup="false" name="C8">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Colour_tag_item.UCS_item.action C7"/>
          </Rhs>
        </Row>
        <Row popup="false" name="C9">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Colour_tag_item.sRGB_item.action C8"/>
          </Rhs>
        </Row>
        <Row popup="false" name="C11">
          <Rhs vislevel="2" flags="4">
            <iText formula="if min (C9 == C1) != 255 then error &quot;fail&quot; else &quot;ok!&quot;"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="0" y="3855" open="true" selected="false" sform="false" next="7" name="D" caption="Colour / Colour Temperature / Move Whitepoint">
      <Subcolumn vislevel="3">
        <Row popup="false" name="D1">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="B1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="D6">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Colour_convert_item.Lab_item.action D1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="D2">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Colour_temperature_item.Whitepoint_item.action D6"/>
          </Rhs>
        </Row>
        <Row popup="false" name="D3">
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
              <Row name="old_white">
                <Rhs vislevel="1" flags="1">
                  <Option caption="Old whitepoint" labelsn="10" labels0="D93" labels1="D75" labels2="D65" labels3="D55" labels4="D50" labels5="A" labels6="B" labels7="C" labels8="E" labels9="D3250" value="4"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="new_white">
                <Rhs vislevel="1" flags="1">
                  <Option caption="New whitepoint" labelsn="10" labels0="D93" labels1="D75" labels2="D65" labels3="D55" labels4="D50" labels5="A" labels6="B" labels7="C" labels8="E" labels9="D3250" value="2"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Colour_temperature_item.Whitepoint_item.action D2"/>
          </Rhs>
        </Row>
        <Row popup="false" name="D4">
          <Rhs vislevel="0" flags="4">
            <iImage window_x="660" window_y="145" window_width="512" window_height="729" image_left="240" image_top="300" image_mag="1" show_status="true" show_paintbox="false" show_convert="true" show_rulers="false" scale="255" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Colour_dE_item.CIEdE76_item.action D3 D1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="D5">
          <Rhs vislevel="1" flags="4">
            <iText formula="if max D4 &gt; 0.01 then error &quot;fail&quot; else &quot;ok!&quot;"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="0" y="4553" open="true" selected="false" sform="false" next="13" name="E" caption="Colour / Colour Temperature">
      <Subcolumn vislevel="3">
        <Row popup="false" name="E1">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="B1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="E2">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Colour_convert_item.Lab_item.action E1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="E3">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Colour_temperature_item.D65_to_D50_item.XYZ_minimal_item.action E2"/>
          </Rhs>
        </Row>
        <Row popup="false" name="E4">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Colour_temperature_item.D50_to_D65_item.XYZ_minimal_item.action E3"/>
          </Rhs>
        </Row>
        <Row popup="false" name="E5">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Colour_temperature_item.D65_to_D50_item.Bradford_item.action E4"/>
          </Rhs>
        </Row>
        <Row popup="false" name="E6">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Colour_temperature_item.D50_to_D65_item.Bradford_item.action E5"/>
          </Rhs>
        </Row>
        <Row popup="false" name="E7">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Colour_temperature_item.Lab_to_D50XYZ_item.action E6"/>
          </Rhs>
        </Row>
        <Row popup="false" name="E8">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Colour_temperature_item.D50XYZ_to_Lab_item.action E7"/>
          </Rhs>
        </Row>
        <Row popup="false" name="E12">
          <Rhs vislevel="0" flags="4">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Colour_dE_item.CIEdE76_item.action E8 E2"/>
          </Rhs>
        </Row>
        <Row popup="false" name="E11">
          <Rhs vislevel="1" flags="4">
            <iText formula="if max E12 &gt; 0.01 then error &quot;fail&quot; else &quot;ok!&quot;"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="0" y="5373" open="true" selected="false" sform="false" next="8" name="F" caption="Colour / ICC">
      <Subcolumn vislevel="3">
        <Row popup="false" name="F1">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="B1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="F2">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
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
              <Row name="profile">
                <Rhs vislevel="1" flags="1">
                  <Pathname/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="intent">
                <Rhs vislevel="1" flags="1">
                  <Option caption="Render intent" labelsn="4" labels0="Perceptual" labels1="Relative" labels2="Saturation" labels3="Absolute" value="1"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="embedded">
                <Rhs vislevel="1" flags="1">
                  <Toggle/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Colour_icc_item.Import_item.action F1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="F3">
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
              <Row name="profile">
                <Rhs vislevel="1" flags="1">
                  <Pathname caption="Output profile" value="$VIPSHOME/share/nip2/data/sRGB.icm"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="intent">
                <Rhs vislevel="1" flags="1">
                  <Option caption="Render intent" labelsn="4" labels0="Perceptual" labels1="Relative" labels2="Saturation" labels3="Absolute" value="1"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="depth">
                <Rhs vislevel="1" flags="1">
                  <Option/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Colour_icc_item.Export_item.action F2"/>
          </Rhs>
        </Row>
        <Row popup="false" name="F4">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
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
              <Row name="in_profile">
                <Rhs vislevel="1" flags="1">
                  <Pathname/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="out_profile">
                <Rhs vislevel="1" flags="1">
                  <Pathname caption="Output profile" value="$VIPSHOME/share/nip2/data/sRGB.icm"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="intent">
                <Rhs vislevel="1" flags="1">
                  <Option/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Colour_icc_item.Transform_item.action F1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="F5">
          <Rhs vislevel="0" flags="4">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Colour_dE_item.CIEdE76_item.action F3 F4"/>
          </Rhs>
        </Row>
        <Row popup="false" name="F6">
          <Rhs vislevel="1" flags="4">
            <iText formula="if max F5 &gt; 2 then error &quot;fail&quot; else &quot;ok!&quot;"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="0" y="6194" open="true" selected="false" sform="false" next="12" name="G" caption="Colour / Difference">
      <Subcolumn vislevel="3">
        <Row popup="false" name="G1">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="B1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="G2">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Colour_convert_item.Lab_item.action G1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="G8">
          <Rhs vislevel="0" flags="4">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="G2"/>
          </Rhs>
        </Row>
        <Row popup="false" name="G3">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Colour_dE_item.CIEdE76_item.action G1 G2"/>
          </Rhs>
        </Row>
        <Row popup="false" name="G4">
          <Rhs vislevel="1" flags="4">
            <iText formula="max G3"/>
          </Rhs>
        </Row>
        <Row popup="false" name="G6">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Colour_dE_item.UCS_item.action G2 G1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="G7">
          <Rhs vislevel="1" flags="4">
            <iText formula="max G6"/>
          </Rhs>
        </Row>
        <Row popup="false" name="G9">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Colour_dE_item.CIEdE00_item.action G2 G8"/>
          </Rhs>
        </Row>
        <Row popup="false" name="G10">
          <Rhs vislevel="1" flags="4">
            <iText formula="max G9"/>
          </Rhs>
        </Row>
        <Row popup="false" name="G11">
          <Rhs vislevel="1" flags="4">
            <iText formula="if G4 &gt; 0.01 || G7 &gt; 0.01 || G10 &gt; 0.01 then error &quot;fail&quot; else &quot;ok!&quot;"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="0" y="6879" open="true" selected="false" sform="false" next="28" name="I" caption="Colour / Adjust">
      <Subcolumn vislevel="3">
        <Row popup="false" name="I1">
          <Rhs vislevel="1" flags="1">
            <iImage window_x="0" window_y="30" window_width="512" window_height="729" image_left="240" image_top="318" image_mag="1" show_status="true" show_paintbox="false" show_convert="false" show_rulers="false" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="B1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="I2">
          <Rhs vislevel="3" flags="7">
            <iImage window_x="0" window_y="30" window_width="512" window_height="729" image_left="240" image_top="318" image_mag="1" show_status="true" show_paintbox="false" show_convert="false" show_rulers="false" scale="1" offset="0" falsecolour="false" type="true"/>
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
              <Row name="matrix">
                <Rhs vislevel="1" flags="1">
                  <Matrix valuen="9" value0="1.1000000000000001" value1="0" value2="0" value3="0" value4="0.90000000000000002" value5="0" value6="0" value7="0" value8="1" width="3" height="3" scale="1" offset="0" filename="" display="1"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Colour_adjust_item.Recombination_item.action I1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="I3">
          <Rhs vislevel="3" flags="7">
            <iImage window_x="0" window_y="30" window_width="512" window_height="729" image_left="240" image_top="318" image_mag="1" show_status="true" show_paintbox="false" show_convert="false" show_rulers="false" scale="1" offset="0" falsecolour="false" type="true"/>
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
              <Row name="gr">
                <Rhs vislevel="1" flags="1">
                  <Slider caption="" from="-20" to="20" value="4"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="by">
                <Rhs vislevel="1" flags="1">
                  <Slider caption="" from="-20" to="20" value="-4"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Colour_adjust_item.Cast_item.action I1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="I5">
          <Rhs vislevel="3" flags="7">
            <iImage window_x="0" window_y="30" window_width="512" window_height="729" image_left="240" image_top="318" image_mag="1" show_status="true" show_paintbox="false" show_convert="false" show_rulers="false" scale="1" offset="0" falsecolour="false" type="true"/>
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
              <Row name="h">
                <Rhs vislevel="1" flags="1">
                  <Slider caption="" from="0" to="360" value="90"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="s">
                <Rhs vislevel="1" flags="1">
                  <Slider/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="b">
                <Rhs vislevel="1" flags="1">
                  <Slider/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Colour_adjust_item.HSB_item.action I1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="I6">
          <Rhs vislevel="1" flags="1">
            <iRegion image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true" left="10" top="10" width="10" height="10">
              <iRegiongroup/>
            </iRegion>
            <Subcolumn vislevel="0"/>
            <iText formula="Region I1 47 23 53 50"/>
          </Rhs>
        </Row>
        <Row popup="false" name="I8">
          <Rhs vislevel="1" flags="1">
            <Colour/>
            <Subcolumn vislevel="0"/>
            <iText formula="Colour_to_colour_item.action I6"/>
          </Rhs>
        </Row>
        <Row popup="false" name="I9">
          <Rhs vislevel="1" flags="1">
            <Colour/>
            <Subcolumn vislevel="0"/>
            <iText formula="I8 * Vector [1.1, 0.9, 1]"/>
          </Rhs>
        </Row>
        <Row popup="false" name="I10">
          <Rhs vislevel="1" flags="1">
            <iRegion image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true" left="10" top="10" width="10" height="10">
              <iRegiongroup/>
            </iRegion>
            <Subcolumn vislevel="0"/>
            <iText formula="Region I2 20 18 22 19"/>
          </Rhs>
        </Row>
        <Row popup="false" name="I11">
          <Rhs vislevel="1" flags="1">
            <Colour/>
            <Subcolumn vislevel="0"/>
            <iText formula="Colour_to_colour_item.action I10"/>
          </Rhs>
        </Row>
        <Row popup="false" name="I12">
          <Rhs vislevel="1" flags="4">
            <iText formula="Colour_dE_item.CIEdE76_item.action I11 I9"/>
          </Rhs>
        </Row>
        <Row popup="false" name="I13">
          <Rhs vislevel="1" flags="4">
            <iText formula="if I12 &gt; 0.0001 then error &quot;fail&quot; else &quot;ok!&quot;"/>
          </Rhs>
        </Row>
        <Row popup="false" name="I14">
          <Rhs vislevel="3" flags="7">
            <Colour/>
            <Subcolumn vislevel="1"/>
            <iText formula="Colour_convert_item.Lab_item.action I8"/>
          </Rhs>
        </Row>
        <Row popup="false" name="I15">
          <Rhs vislevel="1" flags="1">
            <Colour/>
            <Subcolumn vislevel="0"/>
            <iText formula="I14 + Vector [0, 4, -4]"/>
          </Rhs>
        </Row>
        <Row popup="false" name="I16">
          <Rhs vislevel="1" flags="1">
            <iRegion image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true" left="10" top="10" width="10" height="10">
              <iRegiongroup/>
            </iRegion>
            <Subcolumn vislevel="0"/>
            <iText formula="Region I3 34 27 23 17"/>
          </Rhs>
        </Row>
        <Row popup="false" name="I17">
          <Rhs vislevel="1" flags="1">
            <Colour/>
            <Subcolumn vislevel="0"/>
            <iText formula="Colour_to_colour_item.action I16"/>
          </Rhs>
        </Row>
        <Row popup="false" name="I18">
          <Rhs vislevel="3" flags="7">
            <Colour/>
            <Subcolumn vislevel="1"/>
            <iText formula="Colour_convert_item.Lab_item.action I17"/>
          </Rhs>
        </Row>
        <Row popup="false" name="I19">
          <Rhs vislevel="1" flags="4">
            <iText formula="Colour_dE_item.CIEdE76_item.action I18 I15"/>
          </Rhs>
        </Row>
        <Row popup="false" name="I20">
          <Rhs vislevel="1" flags="4">
            <iText formula="if I19 &gt; 0.7 then error &quot;fail&quot; else &quot;ok!&quot;"/>
          </Rhs>
        </Row>
        <Row popup="false" name="I21">
          <Rhs vislevel="1" flags="1">
            <iRegion image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true" left="10" top="10" width="10" height="10">
              <iRegiongroup/>
            </iRegion>
            <Subcolumn vislevel="0"/>
            <iText formula="Region I5 19 27 26 33"/>
          </Rhs>
        </Row>
        <Row popup="false" name="I22">
          <Rhs vislevel="1" flags="1">
            <Colour/>
            <Subcolumn vislevel="0"/>
            <iText formula="Colour_to_colour_item.action I21"/>
          </Rhs>
        </Row>
        <Row popup="false" name="I23">
          <Rhs vislevel="3" flags="7">
            <Colour/>
            <Subcolumn vislevel="1"/>
            <iText formula="Colour_convert_item.LCh_item.action I14"/>
          </Rhs>
        </Row>
        <Row popup="false" name="I24">
          <Rhs vislevel="1" flags="1">
            <Colour/>
            <Subcolumn vislevel="0"/>
            <iText formula="I23 + Vector [0, 0, 90]"/>
          </Rhs>
        </Row>
        <Row popup="false" name="I25">
          <Rhs vislevel="3" flags="7">
            <Colour/>
            <Subcolumn vislevel="1"/>
            <iText formula="Colour_convert_item.sRGB_item.action I24"/>
          </Rhs>
        </Row>
        <Row popup="false" name="I26">
          <Rhs vislevel="1" flags="4">
            <iText formula="Colour_dE_item.CIEdE76_item.action I25 I22"/>
          </Rhs>
        </Row>
        <Row popup="false" name="I27">
          <Rhs vislevel="1" flags="4">
            <iText formula="if I26 &gt; 0.1 then error &quot;fail&quot; else &quot;ok!&quot;"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="0" y="9223" open="true" selected="false" sform="false" next="7" name="J" caption="Colour / Similar Colour">
      <Subcolumn vislevel="3">
        <Row popup="false" name="J1">
          <Rhs vislevel="1" flags="1">
            <iImage window_x="0" window_y="30" window_width="879" window_height="729" image_left="423" image_top="327" image_mag="1" show_status="true" show_paintbox="false" show_convert="false" show_rulers="false" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="B1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="J3">
          <Rhs vislevel="1" flags="1">
            <iRegion image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true" left="10" top="10" width="10" height="10">
              <iRegiongroup/>
            </iRegion>
            <Subcolumn vislevel="0"/>
            <iText formula="Region J1 21 33 22 16"/>
          </Rhs>
        </Row>
        <Row popup="false" name="J4">
          <Rhs vislevel="1" flags="1">
            <Colour/>
            <Subcolumn vislevel="0"/>
            <iText formula="Colour_to_colour_item.action J3"/>
          </Rhs>
        </Row>
        <Row popup="false" name="J2">
          <Rhs vislevel="3" flags="7">
            <iImage window_x="0" window_y="30" window_width="512" window_height="729" image_left="240" image_top="318" image_mag="1" show_status="true" show_paintbox="false" show_convert="false" show_rulers="false" scale="1" offset="0" falsecolour="false" type="true"/>
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
              <Row name="target_colour">
                <Rhs vislevel="3" flags="7">
                  <Colour/>
                  <Subcolumn vislevel="1"/>
                  <iText formula="J4"/>
                </Rhs>
              </Row>
              <Row name="t">
                <Rhs vislevel="1" flags="1">
                  <Slider/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Colour_similar_item.action J1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="J5">
          <Rhs vislevel="1" flags="1">
            <iRegion image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true" left="10" top="10" width="10" height="10">
              <iRegiongroup/>
            </iRegion>
            <Subcolumn vislevel="0"/>
            <iText formula="Region J2 28 23 28 33"/>
          </Rhs>
        </Row>
        <Row popup="false" name="J6">
          <Rhs vislevel="1" flags="4">
            <iText formula="if min J5 == 0 then error &quot;fail&quot; else &quot;ok!&quot;"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="0" y="9886" open="true" selected="false" sform="false" next="12" name="L" caption="Colour / Colour Chart">
      <Subcolumn vislevel="3">
        <Row popup="false" name="L1">
          <Rhs vislevel="2" flags="5">
            <iImage window_x="0" window_y="30" window_width="565" window_height="750" image_left="266" image_top="328" image_mag="1" show_status="true" show_paintbox="false" show_convert="false" show_rulers="false" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Image_file &quot;$VIPSHOME/share/nip2/data/examples/print_test_image.v&quot;"/>
          </Rhs>
        </Row>
        <Row popup="false" name="L2">
          <Rhs vislevel="1" flags="1">
            <iRegion image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true" left="0" top="1" width="549" height="365">
              <iRegiongroup/>
            </iRegion>
            <Subcolumn vislevel="0"/>
            <iText formula="Region L1 18 17 531 349"/>
          </Rhs>
        </Row>
        <Row popup="false" name="L3">
          <Rhs vislevel="0" flags="4">
            <Matrix/>
            <Subcolumn vislevel="0"/>
            <iText formula="Colour_chart_to_matrix_item.action L2"/>
          </Rhs>
        </Row>
        <Row popup="false" name="L4">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Colour_matrix_to_chart_item.action L3"/>
          </Rhs>
        </Row>
        <Row popup="false" name="L7">
          <Rhs vislevel="0" flags="4">
            <Matrix/>
            <Subcolumn vislevel="0"/>
            <iText formula="Colour_chart_to_matrix_item.action L4"/>
          </Rhs>
        </Row>
        <Row popup="false" name="L8">
          <Rhs vislevel="0" flags="4">
            <Matrix/>
            <Subcolumn vislevel="0"/>
            <iText formula="L3 - L7"/>
          </Rhs>
        </Row>
        <Row popup="false" name="L9">
          <Rhs vislevel="1" flags="4">
            <iText formula="max L8"/>
          </Rhs>
        </Row>
        <Row popup="false" name="L10">
          <Rhs vislevel="1" flags="4">
            <iText formula="if L9 &gt; 0.001 then error &quot;fail&quot; else &quot;ok!&quot;"/>
          </Rhs>
        </Row>
        <Row popup="false" name="L11">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Colour_plot_ab_scatter_item.action L2"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="777" y="0" open="true" selected="false" sform="false" next="18" name="H" caption="Filter / Convolution">
      <Subcolumn vislevel="3">
        <Row popup="false" name="H1">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="B1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="H9">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Filter_conv_item.Blur_item.action H1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="H10">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Filter_conv_item.Sharpen_item.action H1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="H11">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Filter_conv_item.Emboss_item.action H1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="H12">
          <Rhs vislevel="1" flags="1">
            <iImage window_x="0" window_y="30" window_width="512" window_height="729" image_left="240" image_top="318" image_mag="1" show_status="true" show_paintbox="false" show_convert="false" show_rulers="false" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Filter_conv_item.Laplacian_item.action H1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="H13">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Filter_conv_item.Sobel_item.action H1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="H14">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Filter_conv_item.Linedet_item.action H1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="H15">
          <Rhs vislevel="3" flags="7">
            <iImage window_x="0" window_y="30" window_width="512" window_height="729" image_left="240" image_top="318" image_mag="1" show_status="true" show_paintbox="false" show_convert="false" show_rulers="false" scale="1" offset="0" falsecolour="false" type="true"/>
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
              <Row name="size">
                <Rhs vislevel="1" flags="1">
                  <Option caption="Radius" labelsn="6" labels0="3 pixels" labels1="5 pixels" labels2="7 pixels" labels3="9 pixels" labels4="11 pixels" labels5="51 pixels" value="4"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="st">
                <Rhs vislevel="1" flags="1">
                  <Slider/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="bm">
                <Rhs vislevel="1" flags="1">
                  <Slider/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="dm">
                <Rhs vislevel="1" flags="1">
                  <Slider/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="fs">
                <Rhs vislevel="1" flags="1">
                  <Slider/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="js">
                <Rhs vislevel="1" flags="1">
                  <Slider/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Filter_conv_item.Usharp_item.action H1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="H16">
          <Rhs vislevel="3" flags="7">
            <iImage window_x="0" window_y="30" window_width="512" window_height="729" image_left="240" image_top="318" image_mag="1" show_status="true" show_paintbox="false" show_convert="false" show_rulers="false" scale="1" offset="0" falsecolour="false" type="true"/>
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
              <Row name="r">
                <Rhs vislevel="1" flags="1">
                  <Slider caption="Radius" from="1" to="100" value="27"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="shape">
                <Rhs vislevel="1" flags="1">
                  <Option caption="Mask shape" labelsn="2" labels0="Square" labels1="Gaussian" value="1"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="type">
                <Rhs vislevel="1" flags="1">
                  <Option/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="fac">
                <Rhs vislevel="1" flags="1">
                  <Slider/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="prec">
                <Rhs vislevel="1" flags="1">
                  <Option/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Filter_conv_item.Custom_blur_item.action H1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="H17">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Filter_conv_item.Custom_conv_item.action H1"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="777" y="1418" open="true" selected="false" sform="false" next="6" name="K" caption="Filter / Rank">
      <Subcolumn vislevel="3">
        <Row popup="false" name="K1">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="B1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="K2">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Filter_rank_item.Median_item.action K1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="K3">
          <Rhs vislevel="1" flags="1">
            <Group/>
            <Subcolumn vislevel="0"/>
            <iText formula="Group [K1,K2]"/>
          </Rhs>
        </Row>
        <Row popup="false" name="K4">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Filter_rank_item.Image_rank_item.action K3"/>
          </Rhs>
        </Row>
        <Row popup="false" name="K5">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Filter_rank_item.Custom_rank_item.action K1"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="777" y="2024" open="true" selected="false" sform="false" next="12" name="M" caption="Filter / Morphology">
      <Subcolumn vislevel="3">
        <Row popup="false" name="M1">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="B1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="M2">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Image_select_item.Threshold_item.action M1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="M3">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Filter_morphology_item.Dilate_item.Dilate8_item.action M2"/>
          </Rhs>
        </Row>
        <Row popup="false" name="M4">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Filter_morphology_item.Dilate_item.Dilate4_item.action M2"/>
          </Rhs>
        </Row>
        <Row popup="false" name="M5">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Filter_morphology_item.Erode_item.Erode8_item.action M2"/>
          </Rhs>
        </Row>
        <Row popup="false" name="M6">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Filter_morphology_item.Erode_item.Erode4_item.action M2"/>
          </Rhs>
        </Row>
        <Row popup="false" name="M7">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Filter_morphology_item.Custom_morph_item.action M2"/>
          </Rhs>
        </Row>
        <Row popup="false" name="M8">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Filter_morphology_item.Open_item.action M2"/>
          </Rhs>
        </Row>
        <Row popup="false" name="M9">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Filter_morphology_item.Close_item.action M2"/>
          </Rhs>
        </Row>
        <Row popup="false" name="M10">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Filter_morphology_item.Clean_item.action M2"/>
          </Rhs>
        </Row>
        <Row popup="false" name="M11">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Filter_morphology_item.Thin_item.action M2"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="777" y="3207" open="true" selected="false" sform="false" next="6" name="N" caption="Filter / Fourier / Ideal">
      <Subcolumn vislevel="3">
        <Row popup="false" name="N1">
          <Rhs vislevel="1" flags="1">
            <iImage window_x="0" window_y="30" window_width="512" window_height="729" image_left="240" image_top="318" image_mag="1" show_status="true" show_paintbox="false" show_convert="false" show_rulers="false" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="B1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="N2">
          <Rhs vislevel="1" flags="1">
            <iRegion image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true" left="113" top="79" width="256" height="256">
              <iRegiongroup/>
            </iRegion>
            <Subcolumn vislevel="0"/>
            <iText formula="Region N1 74 139 264 242"/>
          </Rhs>
        </Row>
        <Row popup="false" name="N3">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
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
              <Row name="sense">
                <Rhs vislevel="1" flags="1">
                  <Option caption="Sense" labelsn="2" labels0="Pass" labels1="Reject" value="1"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="visualize_mask">
                <Rhs vislevel="1" flags="1">
                  <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="fc">
                <Rhs vislevel="1" flags="1">
                  <Slider/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Filter_fourier_item.New_ideal_item.High_low_item.action N2"/>
          </Rhs>
        </Row>
        <Row popup="false" name="N4">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Filter_fourier_item.New_ideal_item.Ring_item.action N2"/>
          </Rhs>
        </Row>
        <Row popup="false" name="N5">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
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
              <Row name="sense">
                <Rhs vislevel="1" flags="1">
                  <Option/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="r">
                <Rhs vislevel="1" flags="1">
                  <Slider caption="Radius" from="0.01" to="0.98999999999999999" value="0.90000000000000002"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="visualize_mask">
                <Rhs vislevel="1" flags="1">
                  <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="fcx">
                <Rhs vislevel="1" flags="1">
                  <Slider/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="fcy">
                <Rhs vislevel="1" flags="1">
                  <Slider/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Filter_fourier_item.New_ideal_item.Band_item.action N2"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="777" y="4269" open="true" selected="false" sform="false" next="9" name="O" caption="Filter / Fourier / Gaussian">
      <Subcolumn vislevel="3">
        <Row popup="false" name="O1">
          <Rhs vislevel="1" flags="1">
            <iImage window_x="0" window_y="30" window_width="512" window_height="729" image_left="240" image_top="318" image_mag="1" show_status="true" show_paintbox="false" show_convert="false" show_rulers="false" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="B1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="O2">
          <Rhs vislevel="1" flags="1">
            <iRegion image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true" left="113" top="79" width="256" height="256">
              <iRegiongroup/>
            </iRegion>
            <Subcolumn vislevel="0"/>
            <iText formula="Region O1 74 139 264 242"/>
          </Rhs>
        </Row>
        <Row popup="false" name="O6">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
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
              <Row name="sense">
                <Rhs vislevel="1" flags="1">
                  <Option caption="Sense" labelsn="2" labels0="Pass" labels1="Reject" value="1"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="visualize_mask">
                <Rhs vislevel="1" flags="1">
                  <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="fc">
                <Rhs vislevel="1" flags="1">
                  <Slider/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="ac">
                <Rhs vislevel="1" flags="1">
                  <Slider/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Filter_fourier_item.New_gaussian_item.High_low_item.action O2"/>
          </Rhs>
        </Row>
        <Row popup="false" name="O7">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Filter_fourier_item.New_gaussian_item.Ring_item.action O2"/>
          </Rhs>
        </Row>
        <Row popup="false" name="O8">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Filter_fourier_item.New_gaussian_item.Band_item.action O2"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="777" y="5421" open="true" selected="false" sform="false" next="9" name="P" caption="Filter / Fourier / Butterworth">
      <Subcolumn vislevel="3">
        <Row popup="false" name="P1">
          <Rhs vislevel="1" flags="1">
            <iImage window_x="0" window_y="30" window_width="512" window_height="729" image_left="240" image_top="318" image_mag="1" show_status="true" show_paintbox="false" show_convert="false" show_rulers="false" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="B1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="P2">
          <Rhs vislevel="1" flags="1">
            <iRegion image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true" left="113" top="79" width="256" height="256">
              <iRegiongroup/>
            </iRegion>
            <Subcolumn vislevel="0"/>
            <iText formula="Region P1 74 139 264 242"/>
          </Rhs>
        </Row>
        <Row popup="false" name="P6">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
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
              <Row name="sense">
                <Rhs vislevel="1" flags="1">
                  <Option caption="Sense" labelsn="2" labels0="Pass" labels1="Reject" value="1"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="visualize_mask">
                <Rhs vislevel="1" flags="1">
                  <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="fc">
                <Rhs vislevel="1" flags="1">
                  <Slider/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="ac">
                <Rhs vislevel="1" flags="1">
                  <Slider/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="o">
                <Rhs vislevel="1" flags="1">
                  <Slider/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Filter_fourier_item.New_butterworth_item.High_low_item.action P2"/>
          </Rhs>
        </Row>
        <Row popup="false" name="P7">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Filter_fourier_item.New_butterworth_item.Ring_item.action P2"/>
          </Rhs>
        </Row>
        <Row popup="false" name="P8">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Filter_fourier_item.New_butterworth_item.Band_item.action P2"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="777" y="6663" open="true" selected="false" sform="false" next="7" name="Q" caption="Filter / Enhance">
      <Subcolumn vislevel="3">
        <Row popup="false" name="Q1">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="B1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="Q2">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Q1?1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="Q3">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Filter_enhance_item.Falsecolour_item.action Q2"/>
          </Rhs>
        </Row>
        <Row popup="false" name="Q4">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Filter_enhance_item.Statistical_diff_item.action Q1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="Q5">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Filter_enhance_item.Hist_equal_item.Global_item.action Q1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="Q6">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Filter_enhance_item.Hist_equal_item.Local_item.action Q1"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="777" y="7611" open="true" selected="false" sform="false" next="7" name="R" caption="Filter / Tilt Brightness">
      <Subcolumn vislevel="3">
        <Row popup="false" name="R1">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="B1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="R2">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
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
              <Row name="tilt">
                <Rhs vislevel="1" flags="1">
                  <Slider caption="Left-right tilt" from="-1" to="1" value="-0.60000000000000009"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Filter_tilt_item.Left_right_item.action R1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="R3">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
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
              <Row name="tilt">
                <Rhs vislevel="1" flags="1">
                  <Slider caption="Top-bottom tilt" from="-1" to="1" value="0.40000000000000002"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Filter_tilt_item.Top_bottom_item.action R1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="R4">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
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
              <Row name="tilt">
                <Rhs vislevel="1" flags="1">
                  <Slider caption="Left-right tilt" from="-1" to="1" value="-0.60000000000000009"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="shift">
                <Rhs vislevel="1" flags="1">
                  <Slider caption="Shift by" from="-1" to="1" value="0.60000000000000009"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Filter_tilt_item.Left_right_cos_item.action R1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="R5">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
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
              <Row name="tilt">
                <Rhs vislevel="1" flags="1">
                  <Slider caption="Top-bottom tilt" from="-1" to="1" value="-0.60000000000000009"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="shift">
                <Rhs vislevel="1" flags="1">
                  <Slider caption="Shift by" from="-1" to="1" value="0.60000000000000009"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Filter_tilt_item.Top_bottom_cos_item.action R1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="R6">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
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
              <Row name="tilt">
                <Rhs vislevel="1" flags="1">
                  <Slider caption="Tilt" from="-1" to="1" value="0.60000000000000009"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="hshift">
                <Rhs vislevel="1" flags="1">
                  <Slider caption="Horizontal shift by" from="-1" to="1" value="-0.40000000000000002"/>
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
            <iText formula="Filter_tilt_item.Circular_item.action R1"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="777" y="8580" open="true" selected="false" sform="false" next="12" name="S" caption="Filter / Blend">
      <Subcolumn vislevel="3">
        <Row popup="false" name="S1">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="B1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="S2">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Q3"/>
          </Rhs>
        </Row>
        <Row popup="false" name="S3">
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
              <Row name="p">
                <Rhs vislevel="1" flags="1">
                  <Slider caption="Blend position" from="0" to="1" value="0.79999999999999993"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Filter_blend_item.Scale_blend_item.action S1 S2"/>
          </Rhs>
        </Row>
        <Row popup="false" name="S4">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="S3?1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="S9">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Image_number_format_item.U8_item.action S4"/>
          </Rhs>
        </Row>
        <Row popup="false" name="S5">
          <Rhs vislevel="3" flags="7">
            <Colour colour_space="sRGB" value0="13" value1="240" value2="65"/>
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
        <Row popup="false" name="S6">
          <Rhs vislevel="3" flags="7">
            <Colour colour_space="sRGB" value0="231" value1="21" value2="13"/>
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
        <Row popup="false" name="S10">
          <Rhs vislevel="1" flags="1">
            <iImage window_x="0" window_y="30" window_width="512" window_height="729" image_left="240" image_top="370" image_mag="1" show_status="true" show_paintbox="false" show_convert="false" show_rulers="false" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Filter_blend_item.Image_blend_item.action S9 S5 S6"/>
          </Rhs>
        </Row>
        <Row popup="false" name="S11">
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
              <Row name="orientation">
                <Rhs vislevel="1" flags="1">
                  <Option/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="blend_position">
                <Rhs vislevel="1" flags="1">
                  <Slider caption="Blend position" from="0" to="1" value="0.59999999999999998"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="blend_width">
                <Rhs vislevel="1" flags="1">
                  <Slider caption="Blend width" from="0" to="1" value="0.34999999999999998"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Filter_blend_item.Line_blend_item.action S1 S2"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="777" y="9822" open="true" selected="false" sform="false" next="5" name="T" caption="Filter / Overlay">
      <Subcolumn vislevel="3">
        <Row popup="false" name="T1">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="B1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="T2">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="T1?0"/>
          </Rhs>
        </Row>
        <Row popup="false" name="T3">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="T1?1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="T4">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
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
                  <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="colour">
                <Rhs vislevel="1" flags="1">
                  <Option caption="Colour overlay as" labelsn="6" labels0="Green over Red" labels1="Blue over Red" labels2="Red over Green" labels3="Red over Blue" labels4="Blue over Green" labels5="Green over Blue" value="3"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Filter_overlay_header_item.action T2 T3"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="777" y="10264" open="true" selected="false" sform="false" next="5" name="U" caption="Filter / Colorize">
      <Subcolumn vislevel="3">
        <Row popup="false" name="U1">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="B1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="U2">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="U1?0"/>
          </Rhs>
        </Row>
        <Row popup="false" name="U3">
          <Rhs vislevel="3" flags="7">
            <Colour colour_space="Lab" value0="61.977499999999999" value1="-8.8122199999999999" value2="59.229700000000001"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Colour_new_item.Widget_colour_item.action"/>
          </Rhs>
        </Row>
        <Row popup="false" name="U4">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Filter_colourize_item.action U2 U3"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="777" y="10802" open="true" selected="false" sform="false" next="4" name="V" caption="Filter / Browse">
      <Subcolumn vislevel="3">
        <Row popup="false" name="V1">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="B1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="V2">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1">
              <Row name="image">
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
              <Row name="band">
                <Rhs vislevel="1" flags="1">
                  <Slider caption="Band" from="0" to="2" value="1.3999999999999999"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="display">
                <Rhs vislevel="1" flags="1">
                  <Option caption="Display as" labelsn="7" labels0="Grey" labels1="Green over Red" labels2="Blue over Red" labels3="Red over Green" labels4="Red over Blue" labels5="Blue over Green" labels6="Green over Blue" value="3"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Filter_browse_multiband_item.Bandwise_item.action V1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="V3">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
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
              <Row name="bit">
                <Rhs vislevel="1" flags="1">
                  <Slider caption="Bit" from="0" to="7" value="4"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Filter_browse_multiband_item.Bitwise_item.action V1"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="777" y="11259" open="true" selected="false" sform="false" next="7" name="W" caption="Filter / Photographic">
      <Subcolumn vislevel="3">
        <Row popup="false" name="W1">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="B1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="W2">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Filter_negative_item.action W1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="W3">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
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
              <Row name="kink">
                <Rhs vislevel="1" flags="1">
                  <Slider caption="Kink" from="0" to="1" value="0.30000000000000004"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Filter_solarize_item.action W1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="W4">
          <Rhs vislevel="3" flags="7">
            <iImage window_x="0" window_y="30" window_width="512" window_height="729" image_left="240" image_top="352" image_mag="1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="1" offset="0" falsecolour="false" type="true"/>
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
              <Row name="r">
                <Rhs vislevel="1" flags="1">
                  <Slider caption="Radius" from="0" to="50" value="10"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="highlights">
                <Rhs vislevel="1" flags="1">
                  <Slider caption="Highlights" from="0" to="100" value="93.893129770992374"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="glow">
                <Rhs vislevel="1" flags="1">
                  <Slider caption="Glow" from="0" to="1" value="0.71183206106870234"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="colour">
                <Rhs vislevel="3" flags="7">
                  <Colour colour_space="Lab" value0="56.0692138671875" value1="-54.071395874023438" value2="43.586296081542969"/>
                  <Subcolumn vislevel="1"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Filter_diffuse_glow_item.action W1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="W5">
          <Rhs vislevel="3" flags="7">
            <iImage window_x="0" window_y="30" window_width="570" window_height="750" image_left="258" image_top="300" image_mag="1" show_status="true" show_paintbox="false" show_convert="true" show_rulers="true" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Filter_drop_shadow_item.action W1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="W6">
          <Rhs vislevel="3" flags="7">
            <iImage window_x="0" window_y="30" window_width="541" window_height="729" image_left="243" image_top="289" image_mag="1" show_status="true" show_paintbox="false" show_convert="true" show_rulers="true" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1">
              <Row name="super">
                <Rhs vislevel="0" flags="4">
                  <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="text">
                <Rhs vislevel="1" flags="1">
                  <String/>
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
              <Row name="align">
                <Rhs vislevel="1" flags="1">
                  <Option/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="dpi">
                <Rhs vislevel="1" flags="1">
                  <Expression caption="DPI"/>
                  <Subcolumn vislevel="0">
                    <Row name="caption">
                      <Rhs vislevel="0" flags="4">
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="expr">
                      <Rhs vislevel="0" flags="4">
                        <iText formula="500"/>
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
              <Row name="colour">
                <Rhs vislevel="1" flags="1">
                  <Expression caption="Text colour"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="place">
                <Rhs vislevel="1" flags="1">
                  <iRegion image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true">
                    <iRegiongroup/>
                  </iRegion>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Filter_paint_text_item.action W1"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="1624" y="0" open="true" selected="false" sform="false" next="12" name="X" caption="Format">
      <Subcolumn vislevel="3">
        <Row popup="false" name="X1">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="B1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="X11">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Object_duplicate_item.action X1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="X4">
          <Rhs vislevel="1" flags="4">
            <iText formula="Object_break_item.action X1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="X6">
          <Rhs vislevel="1" flags="1">
            <iText formula="Object_list_to_group_item.action X4"/>
            <Group/>
            <Subcolumn vislevel="0"/>
          </Rhs>
        </Row>
        <Row popup="false" name="X9">
          <Rhs vislevel="1" flags="4">
            <iText formula="Object_group_to_list_item.action X6"/>
          </Rhs>
        </Row>
        <Row popup="false" name="X10">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Object_assemble_item.action X9"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="3982" y="0" open="true" selected="false" sform="false" next="15" name="Y" caption="Math / Arithmetic">
      <Subcolumn vislevel="3">
        <Row popup="false" name="Y1">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="B1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="Y3">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="B1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="Y4">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Math_arithmetic_item.Add_item.action Y1 Y3"/>
          </Rhs>
        </Row>
        <Row popup="false" name="Y5">
          <Rhs vislevel="2" flags="5">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Math_arithmetic_item.Subtract_item.action Y4 Y3"/>
          </Rhs>
        </Row>
        <Row popup="false" name="Y6">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Math_arithmetic_item.Multiply_item.action Y4 Y5"/>
          </Rhs>
        </Row>
        <Row popup="false" name="Y7">
          <Rhs vislevel="1" flags="4">
            <iText formula="500"/>
          </Rhs>
        </Row>
        <Row popup="false" name="Y8">
          <Rhs vislevel="1" flags="1">
            <iImage window_x="0" window_y="30" window_width="512" window_height="729" image_left="229" image_top="289" image_mag="1" show_status="true" show_paintbox="false" show_convert="true" show_rulers="true" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Math_arithmetic_item.Divide_item.action Y6 Y7"/>
          </Rhs>
        </Row>
        <Row popup="false" name="Y10">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Math_arithmetic_item.Remainder_item.action Y4 Y7"/>
          </Rhs>
        </Row>
        <Row popup="false" name="Y12">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Math_arithmetic_item.Absolute_value_item.action Y10"/>
          </Rhs>
        </Row>
        <Row popup="false" name="Y11">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Math_arithmetic_item.Absolute_value_vector_item.action Y10"/>
          </Rhs>
        </Row>
        <Row popup="false" name="Y13">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Math_arithmetic_item.Sign_item.action Y11"/>
          </Rhs>
        </Row>
        <Row popup="false" name="Y14">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Math_arithmetic_item.Negate_item.action Y13"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="2411" y="0" open="true" selected="false" sform="false" next="19" name="Z" caption="Histogram">
      <Subcolumn vislevel="3">
        <Row popup="false" name="Z2">
          <Rhs vislevel="3" flags="7">
            <Subcolumn vislevel="1"/>
            <iText formula="Hist_new_item.Hist_item.action"/>
            <Plot plot_left="0" plot_top="0" plot_mag="100" show_status="false"/>
          </Rhs>
        </Row>
        <Row popup="false" name="Z1">
          <Rhs vislevel="1" flags="1">
            <iImage window_x="0" window_y="30" window_width="512" window_height="729" image_left="229" image_top="289" image_mag="1" show_status="true" show_paintbox="false" show_convert="true" show_rulers="true" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="B1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="Z3">
          <Rhs vislevel="1" flags="1">
            <Subcolumn vislevel="0"/>
            <iText formula="Hist_find_item.Oned_item.action Z1"/>
            <Plot plot_left="0" plot_top="0" plot_mag="100" show_status="false"/>
          </Rhs>
        </Row>
        <Row popup="false" name="Z4">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Hist_find_item.Nd_item.action Z1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="Z5">
          <Rhs vislevel="1" flags="1">
            <Subcolumn vislevel="0"/>
            <iText formula="Hist_cum_item.action Z3"/>
            <Plot plot_left="0" plot_top="0" plot_mag="100" show_status="false"/>
          </Rhs>
        </Row>
        <Row popup="false" name="Z6">
          <Rhs vislevel="1" flags="1">
            <Subcolumn vislevel="0"/>
            <iText formula="Hist_norm_item.action Z5"/>
            <Plot plot_left="0" plot_top="0" plot_mag="100" show_status="false"/>
          </Rhs>
        </Row>
        <Row popup="false" name="Z7">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Hist_map_item.action Z1 Z6"/>
          </Rhs>
        </Row>
        <Row popup="false" name="Z8">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Hist_eq_item.Global_item.action Z1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="Z9">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Hist_eq_item.Local_item.action Z1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="Z10">
          <Rhs vislevel="1" flags="1">
            <Subcolumn vislevel="0"/>
            <iText formula="Hist_match_item.action Z5 Z3"/>
            <Plot plot_left="0" plot_top="0" plot_mag="100" show_status="false"/>
          </Rhs>
        </Row>
        <Row popup="false" name="Z11">
          <Rhs vislevel="2" flags="5">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Z1?1 &gt; 128"/>
          </Rhs>
        </Row>
        <Row popup="false" name="Z13">
          <Rhs vislevel="3" flags="7">
            <Subcolumn vislevel="1">
              <Row name="x">
                <Rhs vislevel="0" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="super">
                <Rhs vislevel="0" flags="4">
                  <Subcolumn vislevel="0"/>
                  <iText/>
                  <Plot plot_left="0" plot_top="0" plot_mag="100" show_status="false"/>
                </Rhs>
              </Row>
              <Row name="edge">
                <Rhs vislevel="1" flags="1">
                  <Option caption="Search from" labelsn="4" labels0="Top edge down" labels1="Left edge to right" labels2="Bottom edge up" labels3="Right edge to left" value="0"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Hist_profile_item.action Z11"/>
            <Plot plot_left="0" plot_top="0" plot_mag="100" show_status="false"/>
          </Rhs>
        </Row>
        <Row popup="false" name="Z16">
          <Rhs vislevel="1" flags="1">
            <iArrow>
              <iRegiongroup/>
            </iArrow>
            <Subcolumn vislevel="0"/>
            <iText formula="VGuide Z1 135"/>
          </Rhs>
        </Row>
        <Row popup="false" name="Z17">
          <Rhs vislevel="1" flags="1">
            <Subcolumn vislevel="0"/>
            <iText formula="Hist_graph_item.action Z16"/>
            <Plot plot_left="0" plot_top="0" plot_mag="100" show_status="false"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="3197" y="0" open="true" selected="false" sform="false" next="8" name="AB" caption="Image / New">
      <Subcolumn vislevel="3">
        <Row popup="false" name="AB1">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1">
              <Row name="super">
                <Rhs vislevel="0" flags="4">
                  <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="nwidth">
                <Rhs vislevel="1" flags="1">
                  <Expression caption="Image width (pixels)"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="nheight">
                <Rhs vislevel="1" flags="1">
                  <Expression caption="Image height (pixels)"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="nbands">
                <Rhs vislevel="1" flags="1">
                  <Expression caption="Image bands"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="format_option">
                <Rhs vislevel="1" flags="1">
                  <Option caption="Image format" labelsn="10" labels0="8-bit unsigned int - UCHAR" labels1="8-bit signed int - CHAR" labels2="16-bit unsigned int - USHORT" labels3="16-bit signed int - SHORT" labels4="32-bit unsigned int - UINT" labels5="32-bit signed int - INT" labels6="32-bit float - FLOAT" labels7="64-bit complex - COMPLEX" labels8="64-bit float - DOUBLE" labels9="128-bit complex - DPCOMPLEX" value="4"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="type_option">
                <Rhs vislevel="1" flags="1">
                  <Option caption="Image type" labelsn="26" labels0="MULTIBAND" labels1="B_W" labels2="LUMINANCE" labels3="XRAY" labels4="IR" labels5="YUV" labels6="RED_ONLY" labels7="GREEN_ONLY" labels8="BLUE_ONLY" labels9="POWER_SPECTRUM" labels10="HISTOGRAM" labels11="LUT" labels12="XYZ" labels13="LAB" labels14="CMC" labels15="CMYK" labels16="LABQ" labels17="RGB" labels18="UCS" labels19="LCH" labels20="LABS" labels21="sRGB" labels22="YXY" labels23="FOURIER" labels24="RGB16" labels25="GREY16" value="2"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="pixel">
                <Rhs vislevel="1" flags="1">
                  <Expression caption="Pixel value"/>
                  <Subcolumn vislevel="0">
                    <Row name="caption">
                      <Rhs vislevel="0" flags="4">
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="expr">
                      <Rhs vislevel="0" flags="4">
                        <iText formula="Vector [42, 255, 42]"/>
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
            <iText formula="Image_new_item.Image_black_item.action"/>
          </Rhs>
        </Row>
        <Row popup="false" name="AB2">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Image_new_item.Image_new_item.Image_new_from_image_item.action AB1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="AB3">
          <Rhs vislevel="1" flags="1">
            <iRegion image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true">
              <iRegiongroup/>
            </iRegion>
            <Subcolumn vislevel="0"/>
            <iText formula="Image_new_item.Image_new_item.Image_region_item.Region_item.action AB1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="AB4">
          <Rhs vislevel="1" flags="1">
            <iArrow>
              <iRegiongroup/>
            </iArrow>
            <Subcolumn vislevel="0"/>
            <iText formula="Image_new_item.Image_new_item.Image_region_item.Mark_item.action AB1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="AB5">
          <Rhs vislevel="1" flags="1">
            <iArrow>
              <iRegiongroup/>
            </iArrow>
            <Subcolumn vislevel="0"/>
            <iText formula="Image_new_item.Image_new_item.Image_region_item.Arrow_item.action AB1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="AB6">
          <Rhs vislevel="1" flags="1">
            <iArrow>
              <iRegiongroup/>
            </iArrow>
            <Subcolumn vislevel="0"/>
            <iText formula="Image_new_item.Image_new_item.Image_region_item.HGuide_item.action AB1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="AB7">
          <Rhs vislevel="1" flags="1">
            <iArrow>
              <iRegiongroup/>
            </iArrow>
            <Subcolumn vislevel="0"/>
            <iText formula="Image_new_item.Image_new_item.Image_region_item.VGuide_item.action AB1"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="3197" y="713" open="true" selected="false" sform="false" next="18" name="BB" caption="Image / Format">
      <Subcolumn vislevel="3">
        <Row popup="false" name="BB1">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="B1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="BB2">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Image_number_format_item.Float_item.action BB1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="BB3">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
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
              <Row name="first">
                <Rhs vislevel="1" flags="1">
                  <Expression caption="Extract from band"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="number">
                <Rhs vislevel="1" flags="1">
                  <Expression caption="Extract this many bands"/>
                  <Subcolumn vislevel="0">
                    <Row name="caption">
                      <Rhs vislevel="0" flags="4">
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="expr">
                      <Rhs vislevel="0" flags="4">
                        <iText formula="2"/>
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
            <iText formula="Image_band_item.Extract_item.action BB2"/>
          </Rhs>
        </Row>
        <Row popup="false" name="BB4">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Image_band_item.Insert_item.action BB2 BB3"/>
          </Rhs>
        </Row>
        <Row popup="false" name="BB5">
          <Rhs vislevel="3" flags="7">
            <iImage window_x="0" window_y="30" window_width="512" window_height="729" image_left="229" image_top="289" image_mag="1" show_status="true" show_paintbox="false" show_convert="true" show_rulers="true" scale="1" offset="0" falsecolour="false" type="true"/>
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
              <Row name="first">
                <Rhs vislevel="1" flags="1">
                  <Expression caption="Delete from band"/>
                  <Subcolumn vislevel="0">
                    <Row name="caption">
                      <Rhs vislevel="0" flags="4">
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="expr">
                      <Rhs vislevel="0" flags="4">
                        <iText formula="3"/>
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
              <Row name="number">
                <Rhs vislevel="1" flags="1">
                  <Expression caption="Delete this many bands"/>
                  <Subcolumn vislevel="0">
                    <Row name="caption">
                      <Rhs vislevel="0" flags="4">
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="expr">
                      <Rhs vislevel="0" flags="4">
                        <iText formula="2"/>
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
            <iText formula="Image_band_item.Delete_item.action BB4"/>
          </Rhs>
        </Row>
        <Row popup="false" name="BB6">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Image_crop_item.action BB5"/>
          </Rhs>
        </Row>
        <Row popup="false" name="BB7">
          <Rhs vislevel="3" flags="7">
            <iImage window_x="0" window_y="30" window_width="512" window_height="729" image_left="229" image_top="289" image_mag="1" show_status="true" show_paintbox="false" show_convert="true" show_rulers="true" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1">
              <Row name="super">
                <Rhs vislevel="0" flags="4">
                  <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="place">
                <Rhs vislevel="1" flags="1">
                  <iRegion image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true" left="199" top="34" width="248" height="344">
                    <iRegiongroup/>
                  </iRegion>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Image_insert_item.action BB5 BB6"/>
          </Rhs>
        </Row>
        <Row popup="false" name="BB9">
          <Rhs vislevel="1" flags="1">
            <iArrow>
              <iRegiongroup/>
            </iArrow>
            <Subcolumn vislevel="0"/>
            <iText formula="Arrow BB7 367 303 (-250) (-238)"/>
          </Rhs>
        </Row>
        <Row popup="false" name="BB10">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
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
              <Row name="control">
                <Rhs vislevel="1" flags="1">
                  <Option caption="Make" labelsn="7" labels0="Selection Brighter" labels1="Selection Darker" labels2="Selection Black" labels3="Selection White" labels4="Background Black" labels5="Background White" labels6="Mask" value="1"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="width">
                <Rhs vislevel="1" flags="1">
                  <Slider caption="Width" from="0.01" to="1" value="0.30200000000000005"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Image_select_item.Elipse.action BB9"/>
          </Rhs>
        </Row>
        <Row popup="false" name="BB11">
          <Rhs vislevel="1" flags="1">
            <iArrow>
              <iRegiongroup/>
            </iArrow>
            <Subcolumn vislevel="0"/>
            <iText formula="Mark BB7 363 110"/>
          </Rhs>
        </Row>
        <Row popup="false" name="BB12">
          <Rhs vislevel="1" flags="1">
            <iArrow>
              <iRegiongroup/>
            </iArrow>
            <Subcolumn vislevel="0"/>
            <iText formula="Mark BB7 71 268"/>
          </Rhs>
        </Row>
        <Row popup="false" name="BB13">
          <Rhs vislevel="1" flags="1">
            <iArrow>
              <iRegiongroup/>
            </iArrow>
            <Subcolumn vislevel="0"/>
            <iText formula="Mark BB7 198 530"/>
          </Rhs>
        </Row>
        <Row popup="false" name="BB14">
          <Rhs vislevel="1" flags="1">
            <iArrow>
              <iRegiongroup/>
            </iArrow>
            <Subcolumn vislevel="0"/>
            <iText formula="Mark BB7 298 425"/>
          </Rhs>
        </Row>
        <Row popup="false" name="BB15">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
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
              <Row name="c">
                <Rhs vislevel="0" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="d">
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
              <Row name="control">
                <Rhs vislevel="1" flags="1">
                  <Option caption="Make" labelsn="7" labels0="Selection Brighter" labels1="Selection Darker" labels2="Selection Black" labels3="Selection White" labels4="Background Black" labels5="Background White" labels6="Mask" value="1"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Image_select_item.Tetragon.action BB11 BB12 BB13 BB14"/>
          </Rhs>
        </Row>
        <Row popup="false" name="BB16">
          <Rhs vislevel="1" flags="1">
            <iText formula="Group [BB11, BB12, BB13]"/>
            <Group/>
            <Subcolumn vislevel="0"/>
          </Rhs>
        </Row>
        <Row popup="false" name="BB17">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1">
              <Row name="pt_list">
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
              <Row name="control">
                <Rhs vislevel="1" flags="1">
                  <Option caption="Make" labelsn="7" labels0="Selection Brighter" labels1="Selection Darker" labels2="Selection Black" labels3="Selection White" labels4="Background Black" labels5="Background White" labels6="Mask" value="1"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Image_select_item.Polygon.action BB16"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="3197" y="2501" open="true" selected="false" sform="false" next="10" name="CB" caption="Image / Join">
      <Subcolumn vislevel="3">
        <Row popup="false" name="CB1">
          <Rhs vislevel="1" flags="1">
            <iImage window_x="0" window_y="30" window_width="512" window_height="729" image_left="229" image_top="289" image_mag="1" show_status="true" show_paintbox="false" show_convert="true" show_rulers="true" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="B1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="CB2">
          <Rhs vislevel="1" flags="1">
            <iRegion image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true" left="103" top="67" width="147" height="154">
              <iRegiongroup/>
            </iRegion>
            <Subcolumn vislevel="0"/>
            <iText formula="Region CB1 44 189 147 154"/>
          </Rhs>
        </Row>
        <Row popup="false" name="CB3">
          <Rhs vislevel="1" flags="1">
            <iRegion image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true">
              <iRegiongroup/>
            </iRegion>
            <Subcolumn vislevel="0"/>
            <iText formula="Region CB1 275 248 136 141"/>
          </Rhs>
        </Row>
        <Row popup="false" name="CB4">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
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
                  <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="shim">
                <Rhs vislevel="1" flags="1">
                  <Slider caption="Spacing" from="0" to="100" value="78.880407124681938"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="bg_colour">
                <Rhs vislevel="1" flags="1">
                  <Expression caption="Background colour"/>
                  <Subcolumn vislevel="0"/>
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
            </Subcolumn>
            <iText formula="Image_join_item.Left_right_item.action CB2 CB3"/>
          </Rhs>
        </Row>
        <Row popup="false" name="CB5">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
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
                  <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="shim">
                <Rhs vislevel="1" flags="1">
                  <Slider caption="Spacing" from="0" to="100" value="35.368956743002542"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="bg_colour">
                <Rhs vislevel="1" flags="1">
                  <Expression caption="Background colour"/>
                  <Subcolumn vislevel="0"/>
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
            </Subcolumn>
            <iText formula="Image_join_item.Top_bottom_item.action CB4 CB1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="CB6">
          <Rhs vislevel="1" flags="1">
            <iRegion image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true">
              <iRegiongroup/>
            </iRegion>
            <Subcolumn vislevel="0"/>
            <iText formula="Region CB1 19 310 128 132"/>
          </Rhs>
        </Row>
        <Row popup="false" name="CB7">
          <Rhs vislevel="1" flags="1">
            <iRegion image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true">
              <iRegiongroup/>
            </iRegion>
            <Subcolumn vislevel="0"/>
            <iText formula="Region CB1 206 459 130 106"/>
          </Rhs>
        </Row>
        <Row popup="false" name="CB8">
          <Rhs vislevel="1" flags="4">
            <iText formula="[[CB2, CB3],[CB6, CB7]]"/>
          </Rhs>
        </Row>
        <Row popup="false" name="CB9">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
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
              <Row name="hshim">
                <Rhs vislevel="1" flags="1">
                  <Slider caption="Horizontal spacing" from="-100" to="100" value="28.459530026109661"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="vshim">
                <Rhs vislevel="1" flags="1">
                  <Slider caption="Vertical spacing" from="-100" to="100" value="25.848563968668415"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="bg_colour">
                <Rhs vislevel="1" flags="1">
                  <Expression caption="Background colour"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="halign">
                <Rhs vislevel="1" flags="1">
                  <Option/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="valign">
                <Rhs vislevel="1" flags="1">
                  <Option/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Image_join_item.Array_item.action CB8"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="3197" y="3666" open="true" selected="false" sform="false" next="5" name="DB" caption="Image / Tile">
      <Subcolumn vislevel="3">
        <Row popup="false" name="DB1">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="B1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="DB2">
          <Rhs vislevel="3" flags="7">
            <Group/>
            <Subcolumn vislevel="1"/>
            <iText formula="Image_tile_item.Chop_item.action DB1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="DB3">
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
              <Row name="hshim">
                <Rhs vislevel="1" flags="1">
                  <Slider caption="Horizontal spacing" from="-100" to="100" value="40"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="vshim">
                <Rhs vislevel="1" flags="1">
                  <Slider caption="Vertical spacing" from="-100" to="100" value="40"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="bg_colour">
                <Rhs vislevel="1" flags="1">
                  <Expression caption="Background colour"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="halign">
                <Rhs vislevel="1" flags="1">
                  <Option/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="valign">
                <Rhs vislevel="1" flags="1">
                  <Option/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Image_join_item.Array_item.action DB2"/>
          </Rhs>
        </Row>
        <Row popup="false" name="DB4">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1">
              <Row name="default_type">
                <Rhs vislevel="0" flags="4">
                  <iText/>
                </Rhs>
              </Row>
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
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="repeat">
                <Rhs vislevel="1" flags="1">
                  <Option caption="Tile type" labelsn="2" labels0="Replicate" labels1="Four-way mirror" value="1"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Image_tile_item.Replicate_item.action DB1"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="3197" y="4463" open="true" selected="false" sform="false" next="6" name="EB" caption="Image / Levels">
      <Subcolumn vislevel="3">
        <Row popup="false" name="EB1">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="B1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="EB2">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
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
              <Row name="scale">
                <Rhs vislevel="1" flags="1">
                  <Slider caption="Scale" from="0.001" to="3" value="1.5998000000000001"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="offset">
                <Rhs vislevel="1" flags="1">
                  <Slider/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Image_levels_item.Linear_item.action EB1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="EB3">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Image_levels_item.Scale_item.action EB2"/>
          </Rhs>
        </Row>
        <Row popup="false" name="EB4">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
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
              <Row name="gamma">
                <Rhs vislevel="1" flags="1">
                  <Slider caption="Gamma" from="0.001" to="4" value="2.2693403361344533"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="image_maximum_hint">
                <Rhs vislevel="1" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="im_mx">
                <Rhs vislevel="1" flags="1">
                  <Expression caption="Image maximum"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Image_levels_item.Gamma_item.action EB3"/>
          </Rhs>
        </Row>
        <Row popup="false" name="EB5">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Image_levels_item.Tone_item.action EB4"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="3197" y="5492" open="true" selected="false" sform="false" next="19" name="FB" caption="Image / Transform">
      <Subcolumn vislevel="3">
        <Row popup="false" name="FB1">
          <Rhs vislevel="1" flags="1">
            <iImage window_x="5" window_y="54" window_width="512" window_height="729" image_left="476" image_top="596" image_mag="-2" show_status="true" show_paintbox="false" show_convert="true" show_rulers="true" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="B1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="FB2">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Image_transform_item.Rotate_item.Fixed_item.Rot90_item.action FB1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="FB3">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
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
                  <Slider caption="Angle" from="-180" to="180" value="-59.754098360655732"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Image_transform_item.Rotate_item.Free_item.action FB1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="FB4">
          <Rhs vislevel="1" flags="1">
            <iArrow left="142" top="688" width="36" height="-634">
              <iRegiongroup/>
            </iArrow>
            <Subcolumn vislevel="0"/>
            <iText formula="Arrow FB1 134 680 44 (-626)"/>
          </Rhs>
        </Row>
        <Row popup="false" name="FB5">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Image_transform_item.Rotate_item.Straighten_item.action FB4"/>
          </Rhs>
        </Row>
        <Row popup="false" name="FB6">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Image_transform_item.Flip_item.Left_right_item.action FB5"/>
          </Rhs>
        </Row>
        <Row popup="false" name="FB7">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Image_transform_item.Flip_item.Top_bottom_item.action FB5"/>
          </Rhs>
        </Row>
        <Row popup="false" name="FB8">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
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
              <Row name="xfactor">
                <Rhs vislevel="1" flags="1">
                  <Expression caption="Horizontal scale factor"/>
                  <Subcolumn vislevel="0">
                    <Row name="caption">
                      <Rhs vislevel="0" flags="4">
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="expr">
                      <Rhs vislevel="0" flags="4">
                        <iText formula="0.5"/>
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
              <Row name="yfactor">
                <Rhs vislevel="1" flags="1">
                  <Expression caption="Vertical scale factor"/>
                  <Subcolumn vislevel="0">
                    <Row name="caption">
                      <Rhs vislevel="0" flags="4">
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="expr">
                      <Rhs vislevel="0" flags="4">
                        <iText formula="0.5"/>
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
              <Row name="interp">
                <Rhs vislevel="1" flags="1">
                  <Option/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Image_transform_item.Resize_item.Scale_item.action FB7"/>
          </Rhs>
        </Row>
        <Row popup="false" name="FB9">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Image_transform_item.Resize_item.Size_item.action FB1"/>
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
        <Row popup="false" name="FB15">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1">
              <Row name="x">
                <Rhs vislevel="0" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="y">
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
              <Row name="ap1">
                <Rhs vislevel="1" flags="1">
                  <iArrow>
                    <iRegiongroup/>
                  </iArrow>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="bp1">
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
                  <iArrow>
                    <iRegiongroup/>
                  </iArrow>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="bp2">
                <Rhs vislevel="1" flags="1">
                  <iArrow>
                    <iRegiongroup/>
                  </iArrow>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="refine">
                <Rhs vislevel="1" flags="1">
                  <Toggle caption="Refine selected tie-points" value="true"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="lock">
                <Rhs vislevel="1" flags="1">
                  <Toggle/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Image_transform_item.Match_item.action FB1 FB5"/>
          </Rhs>
        </Row>
        <Row popup="false" name="FB16">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Image_transform_item.Image_perspective_match_item.action FB11 FB1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="FB17">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
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
                        <iText formula="496 * 2"/>
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
                        <iText formula="688 * 2"/>
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
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="position">
                <Rhs vislevel="1" flags="1">
                  <Option caption="Position" labelsn="10" labels0="North-west" labels1="North" labels2="North-east" labels3="West" labels4="Centre" labels5="East" labels6="South-west" labels7="South" labels8="South-east" labels9="Specify in pixels" value="4"/>
                  <Subcolumn vislevel="0"/>
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
            <iText formula="Image_transform_item.Resize_item.Resize_item.Resize_canvas_item.action FB1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="FB18">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Image_header_item.Image_header_item.Image_edit_header_item.action FB17"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="3197" y="8631" open="true" selected="false" sform="false" next="18" name="GB" caption="Image / Patterns">
      <Subcolumn vislevel="3">
        <Row popup="false" name="GB1">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Pattern_images_item.Grey_item.action"/>
          </Rhs>
        </Row>
        <Row popup="false" name="GB2">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Pattern_images_item.Xy_item.action"/>
          </Rhs>
        </Row>
        <Row popup="false" name="GB3">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Pattern_images_item.Gaussian_item.action"/>
          </Rhs>
        </Row>
        <Row popup="false" name="GB4">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Pattern_images_item.Fractal_item.action"/>
          </Rhs>
        </Row>
        <Row popup="false" name="GB5">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Pattern_images_item.Checkerboard_item.action"/>
          </Rhs>
        </Row>
        <Row popup="false" name="GB6">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Pattern_images_item.Grid_item.action"/>
          </Rhs>
        </Row>
        <Row popup="false" name="GB7">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Pattern_images_item.Text_item.action"/>
          </Rhs>
        </Row>
        <Row popup="false" name="GB8">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Pattern_images_item.New_CIELAB_slice_item.action"/>
          </Rhs>
        </Row>
        <Row popup="false" name="GB9">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Pattern_images_item.New_ideal_item.High_low_item.action"/>
          </Rhs>
        </Row>
        <Row popup="false" name="GB10">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Pattern_images_item.New_ideal_item.Ring_item.action"/>
          </Rhs>
        </Row>
        <Row popup="false" name="GB11">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Pattern_images_item.New_ideal_item.Band_item.action"/>
          </Rhs>
        </Row>
        <Row popup="false" name="GB12">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Pattern_images_item.New_gaussian_item.High_low_item.action"/>
          </Rhs>
        </Row>
        <Row popup="false" name="GB13">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Pattern_images_item.New_gaussian_item.Ring_item.action"/>
          </Rhs>
        </Row>
        <Row popup="false" name="GB14">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Pattern_images_item.New_gaussian_item.Band_item.action"/>
          </Rhs>
        </Row>
        <Row popup="false" name="GB15">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Pattern_images_item.New_butterworth_item.High_low_item.action"/>
          </Rhs>
        </Row>
        <Row popup="false" name="GB16">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Pattern_images_item.New_butterworth_item.Ring_item.action"/>
          </Rhs>
        </Row>
        <Row popup="false" name="GB17">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Pattern_images_item.New_butterworth_item.Band_item.action"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="3197" y="13004" open="true" selected="false" sform="false" next="8" name="HB" caption="Image / Test">
      <Subcolumn vislevel="3">
        <Row popup="false" name="HB1">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Test_images_item.Eye_item.action"/>
          </Rhs>
        </Row>
        <Row popup="false" name="HB2">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Test_images_item.Zone_plate.action"/>
          </Rhs>
        </Row>
        <Row popup="false" name="HB3">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Test_images_item.Frequency_test_chart_item.action"/>
          </Rhs>
        </Row>
        <Row popup="false" name="HB4">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Test_images_item.CRT_test_chart_item.action"/>
          </Rhs>
        </Row>
        <Row popup="false" name="HB5">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Test_images_item.Greyscale_chart_item.action"/>
          </Rhs>
        </Row>
        <Row popup="false" name="HB6">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Test_images_item.CMYK_test_chart_item.action"/>
          </Rhs>
        </Row>
        <Row popup="false" name="HB7">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Test_images_item.Colour_atlas_item.action"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="3982" y="990" open="true" selected="false" sform="false" next="15" name="JB" caption="Math / Trig">
      <Subcolumn vislevel="3">
        <Row popup="false" name="JB1">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="B1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="JB2">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Math_trig_item.Sin_item.action JB1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="JB3">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Math_trig_item.Cos_item.action JB2"/>
          </Rhs>
        </Row>
        <Row popup="false" name="JB4">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Math_trig_item.Tan_item.action JB3"/>
          </Rhs>
        </Row>
        <Row popup="false" name="JB5">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Math_trig_item.Atan_item.action JB4"/>
          </Rhs>
        </Row>
        <Row popup="false" name="JB6">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Math_trig_item.Acos_item.action JB5"/>
          </Rhs>
        </Row>
        <Row popup="false" name="JB7">
          <Rhs vislevel="1" flags="1">
            <iImage window_x="0" window_y="30" window_width="512" window_height="729" image_left="229" image_top="289" image_mag="1" show_status="true" show_paintbox="false" show_convert="true" show_rulers="true" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Math_trig_item.Asin_item.action JB6"/>
          </Rhs>
        </Row>
        <Row popup="false" name="JB8">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Math_trig_item.Rad_item.action JB1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="JB9">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Math_trig_item.Deg_item.action JB8"/>
          </Rhs>
        </Row>
        <Row popup="false" name="JB11">
          <Rhs vislevel="1" flags="4">
            <iText formula="10"/>
          </Rhs>
        </Row>
        <Row popup="false" name="JB12">
          <Rhs vislevel="1" flags="4">
            <iText formula="128"/>
          </Rhs>
        </Row>
        <Row popup="false" name="JB13">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="B1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="JB14">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Math_trig_item.Angle_range_item.action JB11 JB12 JB13"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="3982" y="1980" open="true" selected="false" sform="false" next="10" name="KB" caption="Math / Log">
      <Subcolumn vislevel="3">
        <Row popup="false" name="KB1">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="B1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="KB4">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Math_log_item.Log_natural_item.action KB1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="KB5">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Math_log_item.Exponential_item.action KB4"/>
          </Rhs>
        </Row>
        <Row popup="false" name="KB6">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Math_log_item.Log10_item.action KB5"/>
          </Rhs>
        </Row>
        <Row popup="false" name="KB7">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Math_log_item.Exponential10_item.action KB6"/>
          </Rhs>
        </Row>
        <Row popup="false" name="KB8">
          <Rhs vislevel="1" flags="4">
            <iText formula="1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="KB9">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Math_log_item.Raise_to_power_item.action KB7 KB8"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="3982" y="2547" open="true" selected="false" sform="false" next="8" name="LB" caption="Math / Complex">
      <Subcolumn vislevel="3">
        <Row popup="false" name="LB1">
          <Rhs vislevel="1" flags="4">
            <iText formula="(2,42)"/>
          </Rhs>
        </Row>
        <Row popup="false" name="LB2">
          <Rhs vislevel="1" flags="4">
            <iText formula="Math_complex_item.Complex_extract.Real_item.action LB1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="LB3">
          <Rhs vislevel="1" flags="4">
            <iText formula="Math_complex_item.Complex_extract.Imaginary_item.action LB1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="LB4">
          <Rhs vislevel="1" flags="4">
            <iText formula="Math_complex_item.Complex_build_item.action LB2 LB3"/>
          </Rhs>
        </Row>
        <Row popup="false" name="LB5">
          <Rhs vislevel="1" flags="4">
            <iText formula="Math_complex_item.Polar_item.action LB4"/>
          </Rhs>
        </Row>
        <Row popup="false" name="LB6">
          <Rhs vislevel="1" flags="4">
            <iText formula="Math_complex_item.Rectangular_item.action LB5"/>
          </Rhs>
        </Row>
        <Row popup="false" name="LB7">
          <Rhs vislevel="1" flags="4">
            <iText formula="Math_complex_item.Conjugate_item.action LB6"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="3982" y="2844" open="true" selected="false" sform="false" next="19" name="MB" caption="Math / Boolean">
      <Subcolumn vislevel="3">
        <Row popup="false" name="MB1">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="B1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="MB2">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="MB1 &gt; 128"/>
          </Rhs>
        </Row>
        <Row popup="false" name="MB3">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="MB1 &lt; 200"/>
          </Rhs>
        </Row>
        <Row popup="false" name="MB4">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Math_boolean_item.And_item.action MB2 MB3"/>
          </Rhs>
        </Row>
        <Row popup="false" name="MB5">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Math_boolean_item.Or_item.action MB2 MB3"/>
          </Rhs>
        </Row>
        <Row popup="false" name="MB6">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Math_boolean_item.Eor_item.action MB2 MB3"/>
          </Rhs>
        </Row>
        <Row popup="false" name="MB7">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Math_boolean_item.Not_item.action MB6"/>
          </Rhs>
        </Row>
        <Row popup="false" name="MB8">
          <Rhs vislevel="1" flags="4">
            <iText formula="1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="MB9">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Math_boolean_item.Right_shift_item.action MB1 MB8"/>
          </Rhs>
        </Row>
        <Row popup="false" name="MB13">
          <Rhs vislevel="1" flags="4">
            <iText formula="1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="MB14">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Math_boolean_item.Left_shift_item.action MB9 MB13"/>
          </Rhs>
        </Row>
        <Row popup="false" name="MB15">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Math_boolean_item.Band_or_item.action MB2"/>
          </Rhs>
        </Row>
        <Row popup="false" name="MB16">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Math_boolean_item.Band_and_item.action MB2"/>
          </Rhs>
        </Row>
        <Row popup="false" name="MB18">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Math_boolean_item.If_then_else_item.action MB16 MB7 MB6"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="3982" y="3912" open="true" selected="false" sform="false" next="9" name="NB" caption="Math / Relational">
      <Subcolumn vislevel="3">
        <Row popup="false" name="NB1">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="B1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="NB2">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="B1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="NB3">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Math_relational_item.Equal_item.action NB1 NB2"/>
          </Rhs>
        </Row>
        <Row popup="false" name="NB4">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Math_relational_item.Not_equal_item.action NB2 NB3"/>
          </Rhs>
        </Row>
        <Row popup="false" name="NB5">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Math_relational_item.More_item.action NB3 NB4"/>
          </Rhs>
        </Row>
        <Row popup="false" name="NB6">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Math_relational_item.Less_item.action NB4 NB5"/>
          </Rhs>
        </Row>
        <Row popup="false" name="NB7">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Math_relational_item.Less_equal_item.action NB5 NB6"/>
          </Rhs>
        </Row>
        <Row popup="false" name="NB8">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Math_relational_item.More_equal_item.action NB6 NB7"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="3982" y="4602" open="true" selected="false" sform="false" next="23" name="OB" caption="Math / List ">
      <Subcolumn vislevel="3">
        <Row popup="false" name="OB1">
          <Rhs vislevel="1" flags="4">
            <iText formula="[1 .. 10]"/>
          </Rhs>
        </Row>
        <Row popup="false" name="OB2">
          <Rhs vislevel="1" flags="4">
            <iText formula="Math_list_item.Head_item.action OB1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="OB3">
          <Rhs vislevel="1" flags="4">
            <iText formula="Math_list_item.Tail_item.action OB1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="OB4">
          <Rhs vislevel="1" flags="4">
            <iText formula="Math_list_item.Cons_item.action OB2 OB3"/>
          </Rhs>
        </Row>
        <Row popup="false" name="OB5">
          <Rhs vislevel="1" flags="4">
            <iText formula="Math_list_item.Last_item.action OB4"/>
          </Rhs>
        </Row>
        <Row popup="false" name="OB7">
          <Rhs vislevel="1" flags="4">
            <iText formula="Math_list_item.Init_item.action OB4"/>
          </Rhs>
        </Row>
        <Row popup="false" name="OB8">
          <Rhs vislevel="1" flags="4">
            <iText formula="[OB5]"/>
          </Rhs>
        </Row>
        <Row popup="false" name="OB9">
          <Rhs vislevel="1" flags="4">
            <iText formula="Math_list_item.Join_item.action OB7 OB8"/>
          </Rhs>
        </Row>
        <Row popup="false" name="OB10">
          <Rhs vislevel="1" flags="4">
            <iText formula="Math_list_item.Reverse_item.action OB9"/>
          </Rhs>
        </Row>
        <Row popup="false" name="OB11">
          <Rhs vislevel="1" flags="4">
            <iText formula="Math_list_item.Sort_item.action OB10"/>
          </Rhs>
        </Row>
        <Row popup="false" name="OB12">
          <Rhs vislevel="1" flags="4">
            <iText formula="[OB11]"/>
          </Rhs>
        </Row>
        <Row popup="false" name="OB13">
          <Rhs vislevel="1" flags="4">
            <iText formula="Math_list_item.Transpose_list_item.action OB12"/>
          </Rhs>
        </Row>
        <Row popup="false" name="OB14">
          <Rhs vislevel="1" flags="4">
            <iText formula="Math_list_item.Concat_item.action OB13"/>
          </Rhs>
        </Row>
        <Row popup="false" name="OB15">
          <Rhs vislevel="1" flags="4">
            <iText formula="Math_list_item.Length_item.action OB14"/>
          </Rhs>
        </Row>
        <Row popup="false" name="OB17">
          <Rhs vislevel="1" flags="4">
            <iText formula="5"/>
          </Rhs>
        </Row>
        <Row popup="false" name="OB18">
          <Rhs vislevel="1" flags="4">
            <iText formula="Math_list_item.Take_item.action OB17 OB14"/>
          </Rhs>
        </Row>
        <Row popup="false" name="OB19">
          <Rhs vislevel="1" flags="4">
            <iText formula="Math_list_item.Drop_item.action OB17 OB14"/>
          </Rhs>
        </Row>
        <Row popup="false" name="OB20">
          <Rhs vislevel="1" flags="4">
            <iText formula="Math_list_item.Zip_item.action OB18 OB19"/>
          </Rhs>
        </Row>
        <Row popup="false" name="OB21">
          <Rhs vislevel="1" flags="4">
            <iText formula="Math_list_item.Transpose_list_item.action OB20"/>
          </Rhs>
        </Row>
        <Row popup="false" name="OB22">
          <Rhs vislevel="1" flags="4">
            <iText formula="Math_list_item.Concat_item.action OB21"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="3982" y="5328" open="true" selected="false" sform="false" next="5" name="PB" caption="Math / Round">
      <Subcolumn vislevel="3">
        <Row popup="false" name="PB1">
          <Rhs vislevel="1" flags="1">
            <iText formula="B1 + 0.5"/>
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
          </Rhs>
        </Row>
        <Row popup="false" name="PB2">
          <Rhs vislevel="1" flags="1">
            <iText formula="Math_round_item.Ceil_item.action PB1"/>
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
          </Rhs>
        </Row>
        <Row popup="false" name="PB3">
          <Rhs vislevel="1" flags="1">
            <iText formula="Math_round_item.Floor_item.action PB1"/>
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
          </Rhs>
        </Row>
        <Row popup="false" name="PB4">
          <Rhs vislevel="1" flags="1">
            <iText formula="Math_round_item.Rint_item.action PB1"/>
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="3982" y="5706" open="true" selected="false" sform="false" next="6" name="QB" caption="Math / Fourier">
      <Subcolumn vislevel="3">
        <Row popup="false" name="QB1">
          <Rhs vislevel="1" flags="1">
            <iImage window_x="0" window_y="30" window_width="512" window_height="729" image_left="240" image_top="318" image_mag="1" show_status="true" show_paintbox="false" show_convert="false" show_rulers="false" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="B1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="QB2">
          <Rhs vislevel="1" flags="1">
            <iRegion image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true" left="210" top="249" width="128" height="128">
              <iRegiongroup/>
            </iRegion>
            <Subcolumn vislevel="0"/>
            <iText formula="Region QB1 127 171 205 188"/>
          </Rhs>
        </Row>
        <Row popup="false" name="QB3">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Math_fourier_item.Forward_item.action QB2"/>
          </Rhs>
        </Row>
        <Row popup="false" name="QB4">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Math_fourier_item.Reverse_item.action QB3"/>
          </Rhs>
        </Row>
        <Row popup="false" name="QB5">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Math_fourier_item.Rotate_quadrants_item.action QB4"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="3982" y="6162" open="true" selected="false" sform="false" next="16" name="RB" caption="Math / Statistics">
      <Subcolumn vislevel="3">
        <Row popup="false" name="RB1">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="B1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="RB6">
          <Rhs vislevel="1" flags="4">
            <iText formula="Math_stats_item.Min_item.action RB1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="RB5">
          <Rhs vislevel="1" flags="4">
            <iText formula="Math_stats_item.Max_item.action RB1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="RB2">
          <Rhs vislevel="1" flags="4">
            <iText formula="Math_stats_item.Mean_item.action RB1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="RB3">
          <Rhs vislevel="1" flags="4">
            <iText formula="Math_stats_item.Deviation_item.action RB1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="RB4">
          <Rhs vislevel="1" flags="1">
            <Matrix/>
            <Subcolumn vislevel="0"/>
            <iText formula="Math_stats_item.Stats_item.action RB1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="RB7">
          <Rhs vislevel="1" flags="4">
            <iText formula="Math_stats_item.Maxpos_item.action RB1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="RB8">
          <Rhs vislevel="1" flags="4">
            <iText formula="Math_stats_item.Minpos_item.action RB1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="RB9">
          <Rhs vislevel="1" flags="4">
            <iText formula="Math_stats_item.Count_set_item.action RB1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="RB10">
          <Rhs vislevel="1" flags="4">
            <iText formula="Math_stats_item.Count_clear_item.action RB1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="RB12">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="RB1?1 &gt; 128"/>
          </Rhs>
        </Row>
        <Row popup="false" name="RB15">
          <Rhs vislevel="3" flags="7">
            <Number/>
            <Subcolumn vislevel="1"/>
            <iText formula="Math_stats_item.Count_edges_item.action RB12"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="3982" y="6843" open="true" selected="false" sform="false" next="8" name="SB" caption="Math / Base">
      <Subcolumn vislevel="3">
        <Row popup="false" name="SB1">
          <Rhs vislevel="1" flags="4">
            <iText formula="42"/>
          </Rhs>
        </Row>
        <Row popup="false" name="SB2">
          <Rhs vislevel="1" flags="4">
            <iText formula="Math_base_item.Hexadecimal_item.action SB1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="SB3">
          <Rhs vislevel="1" flags="4">
            <iText formula="Math_base_item.Binary_item.action SB1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="SB4">
          <Rhs vislevel="1" flags="4">
            <iText formula="Math_base_item.Octal_item.action SB1"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="4540" y="0" open="true" selected="false" sform="false" next="7" name="TB" caption="Matrix / New">
      <Subcolumn vislevel="3">
        <Row popup="false" name="TB1">
          <Rhs vislevel="1" flags="1">
            <Matrix/>
            <Subcolumn vislevel="0"/>
            <iText formula="Matrix_build_item.Plain_item.action"/>
          </Rhs>
        </Row>
        <Row popup="false" name="TB2">
          <Rhs vislevel="1" flags="1">
            <Matrix/>
            <Subcolumn vislevel="0"/>
            <iText formula="Matrix_build_item.Convolution_item.action"/>
          </Rhs>
        </Row>
        <Row popup="false" name="TB3">
          <Rhs vislevel="1" flags="1">
            <Matrix/>
            <Subcolumn vislevel="0"/>
            <iText formula="Matrix_build_item.Recombination_item.action"/>
          </Rhs>
        </Row>
        <Row popup="false" name="TB4">
          <Rhs vislevel="1" flags="1">
            <Matrix/>
            <Subcolumn vislevel="0"/>
            <iText formula="Matrix_build_item.Morphology_item.action"/>
          </Rhs>
        </Row>
        <Row popup="false" name="TB5">
          <Rhs vislevel="3" flags="7">
            <Matrix/>
            <Subcolumn vislevel="1"/>
            <iText formula="Matrix_build_item.Matrix_gaussian_item.action"/>
          </Rhs>
        </Row>
        <Row popup="false" name="TB6">
          <Rhs vislevel="3" flags="7">
            <Matrix/>
            <Subcolumn vislevel="1">
              <Row name="super">
                <Rhs vislevel="0" flags="4">
                  <Matrix/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="integer">
                <Rhs vislevel="1" flags="1">
                  <Toggle caption="Integer" value="true"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="s">
                <Rhs vislevel="1" flags="1">
                  <Slider/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="ma">
                <Rhs vislevel="1" flags="1">
                  <Slider/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Matrix_build_item.Matrix_laplacian_item.action"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="4540" y="1047" open="true" selected="false" sform="false" next="5" name="VB" caption="Matrix / Extract">
      <Subcolumn vislevel="3">
        <Row popup="false" name="VB1">
          <Rhs vislevel="1" flags="1">
            <Matrix/>
            <Subcolumn vislevel="0"/>
            <iText formula="TB1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="VB2">
          <Rhs vislevel="3" flags="7">
            <Matrix/>
            <Subcolumn vislevel="1"/>
            <iText formula="Matrix_extract_item.Rows_item.action VB1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="VB3">
          <Rhs vislevel="3" flags="7">
            <Matrix/>
            <Subcolumn vislevel="1"/>
            <iText formula="Matrix_extract_item.Columns_item.action VB1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="VB4">
          <Rhs vislevel="3" flags="7">
            <Matrix/>
            <Subcolumn vislevel="1"/>
            <iText formula="Matrix_extract_item.Diagonal_item.action VB1"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="4540" y="1625" open="true" selected="false" sform="false" next="4" name="WB" caption="Matrix / Insert">
      <Subcolumn vislevel="3">
        <Row popup="false" name="WB1">
          <Rhs vislevel="1" flags="1">
            <Matrix/>
            <Subcolumn vislevel="0"/>
            <iText formula="TB1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="WB2">
          <Rhs vislevel="3" flags="7">
            <Matrix/>
            <Subcolumn vislevel="1"/>
            <iText formula="Matrix_insert_item.Rows_item.action WB1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="WB3">
          <Rhs vislevel="3" flags="7">
            <Matrix/>
            <Subcolumn vislevel="1"/>
            <iText formula="Matrix_insert_item.Columns_item.action WB1"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="4540" y="2205" open="true" selected="false" sform="false" next="4" name="XB" caption="Matrix / Delete">
      <Subcolumn vislevel="3">
        <Row popup="false" name="XB1">
          <Rhs vislevel="1" flags="1">
            <Matrix/>
            <Subcolumn vislevel="0"/>
            <iText formula="TB1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="XB2">
          <Rhs vislevel="3" flags="7">
            <Matrix/>
            <Subcolumn vislevel="1"/>
            <iText formula="Matrix_delete_item.Rows_item.action XB1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="XB3">
          <Rhs vislevel="3" flags="7">
            <Matrix/>
            <Subcolumn vislevel="1"/>
            <iText formula="Matrix_delete_item.Columns_item.action XB1"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="4540" y="2669" open="true" selected="false" sform="false" next="6" name="YB" caption="Matrix / Rotate">
      <Subcolumn vislevel="3">
        <Row popup="false" name="YB1">
          <Rhs vislevel="1" flags="1">
            <Matrix/>
            <Subcolumn vislevel="0"/>
            <iText formula="TB1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="YB2">
          <Rhs vislevel="3" flags="7">
            <Matrix/>
            <Subcolumn vislevel="1"/>
            <iText formula="Matrix_rotate_item.rot90.action XB1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="YB3">
          <Rhs vislevel="3" flags="7">
            <Matrix/>
            <Subcolumn vislevel="1"/>
            <iText formula="Matrix_rotate_item.rot180.action YB2"/>
          </Rhs>
        </Row>
        <Row popup="false" name="YB4">
          <Rhs vislevel="3" flags="7">
            <Matrix/>
            <Subcolumn vislevel="1"/>
            <iText formula="Matrix_rotate_item.rot270.action YB3"/>
          </Rhs>
        </Row>
        <Row popup="false" name="YB5">
          <Rhs vislevel="1" flags="1">
            <Matrix/>
            <Subcolumn vislevel="0"/>
            <iText formula="Matrix_rotate_item.Matrix_rot45_item.action YB4"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="4540" y="3302" open="true" selected="false" sform="false" next="4" name="ZB" caption="Matrix / Flip">
      <Subcolumn vislevel="3">
        <Row popup="false" name="ZB1">
          <Rhs vislevel="1" flags="1">
            <Matrix/>
            <Subcolumn vislevel="0"/>
            <iText formula="TB1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="ZB2">
          <Rhs vislevel="1" flags="1">
            <Matrix/>
            <Subcolumn vislevel="0"/>
            <iText formula="Matrix_flip_item.Left_right_item.action ZB1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="ZB3">
          <Rhs vislevel="1" flags="1">
            <Matrix/>
            <Subcolumn vislevel="0"/>
            <iText formula="Matrix_flip_item.Top_bottom_item.action ZB2"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="4540" y="3593" open="true" selected="false" sform="false" next="13" name="AC" caption="Matrix">
      <Subcolumn vislevel="3">
        <Row popup="false" name="AC1">
          <Rhs vislevel="1" flags="1">
            <Matrix valuen="9" value0="1" value1="13" value2="42" value3="12" value4="1" value5="2" value6="1" value7="22" value8="1" width="3" height="3" scale="1" offset="0" filename="" display="0"/>
            <Subcolumn vislevel="0"/>
            <iText formula="TB1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="AC2">
          <Rhs vislevel="1" flags="1">
            <Matrix/>
            <Subcolumn vislevel="0"/>
            <iText formula="Matrix_invert_item.action AC1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="AC3">
          <Rhs vislevel="1" flags="1">
            <Matrix/>
            <Subcolumn vislevel="0"/>
            <iText formula="Matrix_transpose_item.action AC2"/>
          </Rhs>
        </Row>
        <Row popup="false" name="AC4">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Image_convert_to_image_item.action AC3"/>
          </Rhs>
        </Row>
        <Row popup="false" name="AC5">
          <Rhs vislevel="1" flags="1">
            <Matrix/>
            <Subcolumn vislevel="0"/>
            <iText formula="Matrix_to_matrix_item.action AC4"/>
          </Rhs>
        </Row>
        <Row popup="false" name="AC6">
          <Rhs vislevel="1" flags="4">
            <iText formula="[1, 10..360]"/>
          </Rhs>
        </Row>
        <Row popup="false" name="AC7">
          <Rhs vislevel="1" flags="4">
            <iText formula="map sin AC6"/>
          </Rhs>
        </Row>
        <Row popup="false" name="AC10">
          <Rhs vislevel="1" flags="1">
            <iText formula="Matrix (zip2 AC6 AC7)"/>
            <Matrix/>
            <Subcolumn vislevel="0"/>
          </Rhs>
        </Row>
        <Row popup="false" name="AC12">
          <Rhs vislevel="3" flags="7">
            <Subcolumn vislevel="1"/>
            <iText formula="Matrix_plot_scatter_item.action AC10"/>
            <Plot plot_left="0" plot_top="0" plot_mag="100" show_status="false"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="5170" y="0" open="true" selected="false" sform="false" next="33" name="BC" caption="Tasks / Capture">
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
    <Column x="5170" y="3148" open="true" selected="false" sform="false" next="31" name="UB" caption="Tasks / Mosaic / Onepoint|Twopoint">
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
    <Column x="6005" y="0" open="true" selected="true" sform="false" next="11" name="CC" caption="Widgets">
      <Subcolumn vislevel="3">
        <Row popup="false" name="CC1">
          <Rhs vislevel="1" flags="1">
            <Slider/>
            <Subcolumn vislevel="0"/>
            <iText formula="Widget_slider_item.action"/>
          </Rhs>
        </Row>
        <Row popup="false" name="CC2">
          <Rhs vislevel="1" flags="1">
            <Toggle/>
            <Subcolumn vislevel="0"/>
            <iText formula="Widget_toggle_item.action"/>
          </Rhs>
        </Row>
        <Row popup="false" name="CC4">
          <Rhs vislevel="1" flags="1">
            <String/>
            <Subcolumn vislevel="0"/>
            <iText formula="Widget_string_item.action"/>
          </Rhs>
        </Row>
        <Row popup="false" name="CC5">
          <Rhs vislevel="1" flags="1">
            <Number/>
            <Subcolumn vislevel="0"/>
            <iText formula="Widget_number_item.action"/>
          </Rhs>
        </Row>
        <Row popup="false" name="CC6">
          <Rhs vislevel="1" flags="1">
            <Expression caption="Enter an expression"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Widget_expression_item.action"/>
          </Rhs>
        </Row>
        <Row popup="false" name="CC7">
          <Rhs vislevel="1" flags="1">
            <Pathname/>
            <Subcolumn vislevel="0"/>
            <iText formula="Widget_pathname_item.action"/>
          </Rhs>
        </Row>
        <Row popup="false" name="CC8">
          <Rhs vislevel="1" flags="1">
            <Fontname/>
            <Subcolumn vislevel="0"/>
            <iText formula="Widget_font_item.action"/>
          </Rhs>
        </Row>
        <Row popup="false" name="CC10">
          <Rhs vislevel="1" flags="1">
            <Clock interval="1" value="1444.7167530440001"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Widget_clock_item.action"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="5170" y="4606" open="true" selected="false" sform="false" next="12" name="IB" caption="Tasks / Mosaic / Manual Balance">
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
    <Column x="5170" y="5872" open="true" selected="false" sform="false" next="6" name="EC" caption="Tasks / Mosaic / Clone">
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
    <Column x="6399" y="0" open="true" selected="false" sform="false" next="6" name="GC" caption="untitled">
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
    <Column x="6399" y="411" open="true" selected="false" sform="false" next="2" name="HC" caption="Straighten Frame">
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
    <Column x="6919" y="0" open="true" selected="false" sform="false" next="2" name="IC" caption="Painting with Simple Frame">
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
    <Column x="6919" y="387" open="true" selected="false" sform="false" next="2" name="JC" caption="Painting with Complex Frame, with adjusted variables">
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
    <Column x="7598" y="0" open="true" selected="false" sform="false" next="2" name="KC" caption="Painting with frame corner, and coloured mount">
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
    <Column x="8189" y="0" open="true" selected="false" sform="false" next="8" name="DC" caption="test im_affine">
      <Subcolumn vislevel="3">
        <Row popup="false" name="DC1">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Image_file &quot;test-5x5.v&quot;"/>
          </Rhs>
        </Row>
        <Row popup="false" name="DC2">
          <Rhs vislevel="3" flags="7">
            <iImage window_x="5" window_y="30" window_width="439" window_height="363" image_left="192" image_top="132" image_mag="1" show_status="true" show_paintbox="false" show_convert="false" show_rulers="true" scale="1" offset="0" falsecolour="false" type="true"/>
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
              <Row name="xfactor">
                <Rhs vislevel="1" flags="1">
                  <Expression caption="Horizontal scale factor"/>
                  <Subcolumn vislevel="0">
                    <Row name="caption">
                      <Rhs vislevel="0" flags="4">
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="expr">
                      <Rhs vislevel="0" flags="4">
                        <iText formula="64"/>
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
              <Row name="yfactor">
                <Rhs vislevel="1" flags="1">
                  <Expression caption="Vertical scale factor"/>
                  <Subcolumn vislevel="0">
                    <Row name="caption">
                      <Rhs vislevel="0" flags="4">
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="expr">
                      <Rhs vislevel="0" flags="4">
                        <iText formula="64"/>
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
              <Row name="interp">
                <Rhs vislevel="1" flags="1">
                  <Option caption="Interpolation" labelsn="3" labels0="Nearest neighbour" labels1="Bilinear" labels2="Bicubic" value="1"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Image_transform_item.Resize_item.Scale_item.action DC1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="DC3">
          <Rhs vislevel="1" flags="1">
            <iArrow>
              <iRegiongroup/>
            </iArrow>
            <Subcolumn vislevel="0"/>
            <iText formula="VGuide DC2 94"/>
          </Rhs>
        </Row>
        <Row popup="false" name="DC4">
          <Rhs vislevel="1" flags="1">
            <iArrow left="0" top="225" width="320" height="0">
              <iRegiongroup/>
            </iArrow>
            <Subcolumn vislevel="0"/>
            <iText formula="HGuide DC2 226"/>
          </Rhs>
        </Row>
        <Row popup="false" name="DC5">
          <Rhs vislevel="1" flags="1">
            <iArrow>
              <iRegiongroup/>
            </iArrow>
            <Subcolumn vislevel="0"/>
            <iText formula="VGuide DC2 225"/>
          </Rhs>
        </Row>
        <Row popup="false" name="DC6">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
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
                  <Slider caption="Angle" from="-180" to="180" value="0"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Image_transform_item.Rotate_item.Free_item.action DC1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="DC7">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
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
                  <Slider caption="Angle" from="-180" to="180" value="90"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Image_transform_item.Rotate_item.Free_item.action DC1"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="0" y="634" open="true" selected="false" sform="false" next="19" name="FC" caption="Colour / Colourspace">
      <Subcolumn vislevel="3">
        <Row popup="false" name="FC1">
          <Rhs vislevel="2" flags="5">
            <iImage window_x="0" window_y="30" window_width="512" window_height="729" image_left="229" image_top="309" image_mag="1" show_status="true" show_paintbox="false" show_convert="true" show_rulers="true" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Image_file &quot;$VIPSHOME/share/nip2/data/examples/businesscard/slanted_oval_vase2.jpg&quot;"/>
          </Rhs>
        </Row>
        <Row popup="false" name="FC16">
          <Rhs vislevel="1" flags="1">
            <Colour/>
            <Subcolumn vislevel="0"/>
            <iText formula="Colour_to_colour_item.action FC1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="FC17">
          <Rhs vislevel="1" flags="4">
            <iText formula="12"/>
          </Rhs>
        </Row>
        <Row popup="false" name="FC18">
          <Rhs vislevel="1" flags="1">
            <Colour/>
            <Subcolumn vislevel="0"/>
            <iText formula="Colour_to_colour_item.action FC17"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
  </Workspace>
</root>



