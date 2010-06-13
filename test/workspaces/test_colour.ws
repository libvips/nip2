<?xml version="1.0"?>
<root xmlns="http://www.vips.ecs.soton.ac.uk/nip/7.18.0">
  <Workspace filename="workspaces/test_colour.ws" view="WORKSPACE_MODE_REGULAR" scale="1" offset="0" window_width="1280" window_height="771" lpane_position="400" lpane_open="false" rpane_position="100" rpane_open="false" local_defs="// private definitions for this workspace&#10;" name="test_colour" caption="Default empty workspace">
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
          <Rhs vislevel="2" flags="5">
            <Colour/>
            <Subcolumn vislevel="0"/>
            <iText formula="Colour_to_colour_item.action A8"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="386" y="366" open="true" selected="false" sform="false" next="22" name="B" caption="Colour / Colourspace">
      <Subcolumn vislevel="3">
        <Row popup="false" name="B1">
          <Rhs vislevel="0" flags="4">
            <iImage window_x="0" window_y="30" window_width="512" window_height="729" image_left="229" image_top="309" image_mag="1" show_status="true" show_paintbox="false" show_convert="true" show_rulers="true" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="FC1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="B19">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Colour_convert_item.Mono_item.action B1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="B18">
          <Rhs vislevel="3" flags="7">
            <iImage window_x="1059" window_y="312" window_width="574" window_height="727" image_left="271" image_top="308" image_mag="1" show_status="true" show_paintbox="false" show_convert="true" show_rulers="false" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Colour_convert_item.GREY16_item.action B19"/>
          </Rhs>
        </Row>
        <Row popup="false" name="B9">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Colour_convert_item.sRGB_item.action B8"/>
          </Rhs>
        </Row>
        <Row popup="false" name="B20">
          <Rhs vislevel="3" flags="7">
            <iImage window_x="1059" window_y="312" window_width="574" window_height="727" image_left="271" image_top="308" image_mag="1" show_status="true" show_paintbox="false" show_convert="true" show_rulers="false" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Colour_convert_item.RGB16_item.action B1"/>
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
            <Subcolumn vislevel="1">
              <Row name="dest">
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
                  <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="to">
                <Rhs vislevel="1" flags="1">
                  <Option caption="Convert to" labelsn="11" labels0="Mono" labels1="sRGB" labels2="RGB16" labels3="GREY16" labels4="Lab" labels5="LabQ" labels6="LabS" labels7="LCh" labels8="XYZ" labels9="Yxy" labels10="UCS" value="9"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
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
        <Row popup="false" name="B15">
          <Rhs vislevel="1" flags="4">
            <iText formula="&quot;test max difference &lt; 2&quot;"/>
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
            <iText formula="if B13 &gt; 2 then error &quot;fail&quot; else &quot;ok!&quot;"/>
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
    <Column x="1042" y="0" open="true" selected="false" sform="false" next="15" name="C" caption="Colour / Tag As">
      <Subcolumn vislevel="3">
        <Row popup="false" name="C1">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="B1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="C12">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Colour_tag_item.Mono_item.action C1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="C9">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Colour_tag_item.sRGB_item.action C8"/>
          </Rhs>
        </Row>
        <Row popup="false" name="C13">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Colour_tag_item.RGB16_item.action C1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="C14">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Colour_tag_item.GREY16_item.action C1"/>
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
        <Row popup="false" name="C11">
          <Rhs vislevel="2" flags="4">
            <iText formula="if min (C9 == C1) != 255 then error &quot;fail&quot; else &quot;ok!&quot;"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="1526" y="0" open="true" selected="false" sform="false" next="7" name="D" caption="Colour / Colour Temperature / Move Whitepoint">
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
    <Column x="2003" y="0" open="true" selected="false" sform="false" next="13" name="E" caption="Colour / Colour Temperature">
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
    <Column x="2486" y="0" open="true" selected="true" sform="false" next="9" name="F" caption="Colour / ICC">
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
        <Row popup="false" name="F8">
          <Rhs vislevel="1" flags="4">
            <iText formula="max F5"/>
          </Rhs>
        </Row>
        <Row popup="false" name="F6">
          <Rhs vislevel="1" flags="4">
            <iText formula="if F8 &gt; 2 then error &quot;fail&quot; else &quot;ok!&quot;"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="2960" y="0" open="true" selected="false" sform="false" next="12" name="G" caption="Colour / Difference">
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
    <Column x="3445" y="0" open="true" selected="false" sform="false" next="28" name="I" caption="Colour / Adjust">
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
    <Column x="4049" y="0" open="true" selected="false" sform="false" next="7" name="J" caption="Colour / Similar Colour">
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
                  <Slider caption="dE threshold" from="0" to="100" value="32"/>
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
    <Column x="4519" y="0" open="true" selected="false" sform="false" next="12" name="L" caption="Colour / Colour Chart">
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
    <Column x="386" y="0" open="true" selected="false" sform="false" next="19" name="FC" caption="Colour / Colourspace">
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



