<?xml version="1.0"?>
<root xmlns="http://www.vips.ecs.soton.ac.uk/nip/8.5.0">
  <Workspace filename="/home/john/SVN/vips/nip2/trunk/test/workspaces/test_histogram.ws" view="WORKSPACE_MODE_REGULAR" scale="1" offset="0" window_width="1920" window_height="1165" lpane_position="400" lpane_open="false" rpane_position="100" rpane_open="false" local_defs="// private definitions for this workspace&#10;" name="test_histogram" caption="Default empty workspace">
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
    <Column x="476" y="0" open="true" selected="true" sform="false" next="35" name="Z" caption="Histogram / Make, find, apply">
      <Subcolumn vislevel="3">
        <Row popup="false" name="Z1">
          <Rhs vislevel="0" flags="4">
            <iImage window_x="4" window_y="53" window_width="547" window_height="729" image_left="257" image_top="309" image_mag="1" show_status="true" show_paintbox="false" show_convert="true" show_rulers="false" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="B1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="Z2">
          <Rhs vislevel="3" flags="7">
            <Subcolumn vislevel="1"/>
            <iText formula="Hist_new_item.Hist_item.action"/>
            <Plot plot_left="0" plot_top="0" plot_mag="100" show_status="false"/>
          </Rhs>
        </Row>
        <Row popup="false" name="Z19">
          <Rhs vislevel="2" flags="5">
            <iText formula="Matrix [[0,0],[128,20],[200,90],[255,255]]"/>
            <Matrix/>
            <Subcolumn vislevel="0"/>
          </Rhs>
        </Row>
        <Row popup="false" name="Z21">
          <Rhs vislevel="1" flags="1">
            <Subcolumn vislevel="0"/>
            <iText formula="Hist_new_item.Hist_new_from_matrix.action Z19"/>
            <Plot plot_left="0" plot_top="0" plot_mag="100" show_status="false"/>
          </Rhs>
        </Row>
        <Row popup="false" name="Z22">
          <Rhs vislevel="1" flags="1">
            <Plot plot_left="0" plot_top="0" plot_mag="100" show_status="false"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Hist_new_item.Hist_from_image_item.action Z21"/>
          </Rhs>
        </Row>
        <Row popup="false" name="Z23">
          <Rhs vislevel="3" flags="7">
            <Plot plot_left="0" plot_top="0" plot_mag="100" show_status="false"/>
            <Subcolumn vislevel="1">
              <Row name="super">
                <Rhs vislevel="0" flags="4">
                  <Plot plot_left="0" plot_top="0" plot_mag="100" show_status="false"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="d">
                <Rhs vislevel="1" flags="1">
                  <Option caption="Depth" labelsn="2" labels0="8 bit" labels1="16 bit" value="0"/>
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
              <Row name="w">
                <Rhs vislevel="1" flags="1">
                  <Slider/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="sp">
                <Rhs vislevel="1" flags="1">
                  <Slider/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="mp">
                <Rhs vislevel="1" flags="1">
                  <Slider/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="hp">
                <Rhs vislevel="1" flags="1">
                  <Slider caption="Highlight point" from="0.69999999999999996" to="0.90000000000000002" value="0.90000000000000002"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="sa">
                <Rhs vislevel="1" flags="1">
                  <Slider caption="Shadow adjust" from="-15" to="15" value="-12.25609756097561"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="ma">
                <Rhs vislevel="1" flags="1">
                  <Slider caption="Mid-tone adjust" from="-30" to="30" value="16.463414634146346"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="ha">
                <Rhs vislevel="1" flags="1">
                  <Slider caption="Highlight adjust" from="-15" to="15" value="-11.524390243902438"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Hist_new_item.Tone_item.action"/>
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
        <Row popup="false" name="Z24">
          <Rhs vislevel="1" flags="1">
            <iImage window_x="749" window_y="29" window_width="510" window_height="727" image_left="248" image_top="344" image_mag="1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Hist_map_item.action Z1 Z23"/>
          </Rhs>
        </Row>
        <Row popup="false" name="Z25">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Hist_eq_item.Global_item.action Z24"/>
          </Rhs>
        </Row>
        <Row popup="false" name="Z26">
          <Rhs vislevel="1" flags="1">
            <iRegion image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true">
              <iRegiongroup/>
            </iRegion>
            <Subcolumn vislevel="0"/>
            <iText formula="Region Z24 48 326 45 46"/>
          </Rhs>
        </Row>
        <Row popup="false" name="Z27">
          <Rhs vislevel="3" flags="7">
            <Subcolumn vislevel="1"/>
            <iText formula="Hist_eq_item.Local_item.action Z1"/>
            <iImage window_x="1050" window_y="189" window_width="510" window_height="727" image_left="248" image_top="344" image_mag="1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="1" offset="0" falsecolour="false" type="true"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="993" y="0" open="true" selected="false" sform="false" next="29" name="A" caption="Histogram / Maths">
      <Subcolumn vislevel="3">
        <Row popup="false" name="A1">
          <Rhs vislevel="0" flags="4">
            <iImage window_x="4" window_y="53" window_width="547" window_height="729" image_left="257" image_top="309" image_mag="1" show_status="true" show_paintbox="false" show_convert="true" show_rulers="false" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="B1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="A13">
          <Rhs vislevel="2" flags="5">
            <Subcolumn vislevel="0"/>
            <iText formula="Hist_cum_item.action Z3"/>
            <Plot plot_left="0" plot_top="0" plot_mag="100" show_status="false"/>
          </Rhs>
        </Row>
        <Row popup="false" name="A14">
          <Rhs vislevel="2" flags="5">
            <Plot plot_left="0" plot_top="0" plot_mag="100" show_status="false"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Hist_diff_item.action A13"/>
          </Rhs>
        </Row>
        <Row popup="false" name="A15">
          <Rhs vislevel="2" flags="4">
            <iText formula="if min (A14 == Z3) == 255 then &quot;ok!&quot; else error &quot;diff (cum a) != a&quot;"/>
          </Rhs>
        </Row>
        <Row popup="false" name="A16">
          <Rhs vislevel="2" flags="5">
            <Subcolumn vislevel="0"/>
            <iText formula="Hist_norm_item.action A13"/>
            <Plot plot_left="0" plot_top="0" plot_mag="100" show_status="false"/>
          </Rhs>
        </Row>
        <Row popup="false" name="A17">
          <Rhs vislevel="2" flags="5">
            <Subcolumn vislevel="0"/>
            <iText formula="Hist_match_item.action A13 Z3"/>
            <Plot plot_left="0" plot_top="0" plot_mag="100" show_status="false"/>
          </Rhs>
        </Row>
        <Row popup="false" name="A27">
          <Rhs vislevel="1" flags="1">
            <Plot plot_left="0" plot_top="0" plot_mag="100" show_status="false"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Hist_diff_item.action A14"/>
          </Rhs>
        </Row>
        <Row popup="false" name="A28">
          <Rhs vislevel="3" flags="7">
            <Plot plot_left="0" plot_top="0" plot_mag="100" show_status="false"/>
            <Subcolumn vislevel="1">
              <Row name="x">
                <Rhs vislevel="0" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="super">
                <Rhs vislevel="0" flags="4">
                  <Plot plot_left="0" plot_top="0" plot_mag="100" show_status="false"/>
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
              <Row name="r">
                <Rhs vislevel="1" flags="1">
                  <Slider caption="Radius" from="1" to="100" value="10.9"/>
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
              <Row name="shape">
                <Rhs vislevel="1" flags="1">
                  <Option caption="Mask shape" labelsn="2" labels0="Square" labels1="Gaussian" value="1"/>
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
            <iText formula="Filter_conv_item.Custom_blur_item.action A27"/>
          </Rhs>
        </Row>
        <Row popup="false" name="A26">
          <Rhs vislevel="3" flags="7">
            <Subcolumn vislevel="1"/>
            <iText formula="Hist_zerox_item.action A28"/>
            <Plot plot_left="0" plot_top="0" plot_mag="100" show_status="false"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="1529" y="0" open="true" selected="false" sform="false" next="15" name="C" caption="Histogram / Profile and Projection">
      <Subcolumn vislevel="3">
        <Row popup="false" name="C1">
          <Rhs vislevel="0" flags="4">
            <iImage window_x="4" window_y="53" window_width="547" window_height="729" image_left="257" image_top="309" image_mag="1" show_status="true" show_paintbox="false" show_convert="true" show_rulers="false" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="B1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="C7">
          <Rhs vislevel="2" flags="5">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="C1?1 &gt; 128"/>
          </Rhs>
        </Row>
        <Row popup="false" name="C8">
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
            <iText formula="Hist_profile_item.action C7"/>
            <Plot plot_left="0" plot_top="0" plot_mag="100" show_status="false"/>
          </Rhs>
        </Row>
        <Row popup="false" name="C9">
          <Rhs vislevel="2" flags="6">
            <Subcolumn vislevel="1"/>
            <iText formula="Hist_project_item.action C1"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="2136" y="0" open="true" selected="false" sform="false" next="10" name="D" caption="Histogram / Extract, Plot">
      <Subcolumn vislevel="3">
        <Row popup="false" name="D1">
          <Rhs vislevel="0" flags="4">
            <iImage window_x="4" window_y="53" window_width="547" window_height="729" image_left="257" image_top="309" image_mag="1" show_status="true" show_paintbox="false" show_convert="true" show_rulers="false" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="B1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="D5">
          <Rhs vislevel="1" flags="1">
            <iArrow>
              <iRegiongroup/>
            </iArrow>
            <Subcolumn vislevel="0"/>
            <iText formula="VGuide D1 135"/>
          </Rhs>
        </Row>
        <Row popup="false" name="D6">
          <Rhs vislevel="1" flags="1">
            <Subcolumn vislevel="0"/>
            <iText formula="Hist_graph_item.action D5"/>
            <Plot plot_left="0" plot_top="0" plot_mag="100" show_status="false"/>
          </Rhs>
        </Row>
        <Row popup="false" name="D7">
          <Rhs vislevel="1" flags="1">
            <iArrow left="395" top="300" width="-197" height="121">
              <iRegiongroup/>
            </iArrow>
            <Subcolumn vislevel="0"/>
            <iText formula="Arrow D1 417 184 (-197) 121"/>
          </Rhs>
        </Row>
        <Row popup="false" name="D8">
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
              <Row name="width">
                <Rhs vislevel="1" flags="1">
                  <Slider caption="Width" from="1" to="40" value="9.1319148936170205"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="displace">
                <Rhs vislevel="1" flags="1">
                  <Slider/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="vdisplace">
                <Rhs vislevel="1" flags="1">
                  <Slider/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Extract_arrow_item.action D7"/>
          </Rhs>
        </Row>
        <Row popup="false" name="D9">
          <Rhs vislevel="3" flags="7">
            <Plot plot_left="0" plot_top="0" plot_mag="100" show_status="false"/>
            <Subcolumn vislevel="1">
              <Row name="x">
                <Rhs vislevel="0" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="super">
                <Rhs vislevel="0" flags="4">
                  <Plot plot_left="0" plot_top="0" plot_mag="100" show_status="false"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="format">
                <Rhs vislevel="1" flags="1">
                  <Option caption="Format" labelsn="3" labels0="YYYY" labels1="XYYY" labels2="XYXY" value="2"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="style">
                <Rhs vislevel="1" flags="1">
                  <Option caption="Style" labelsn="4" labels0="Point" labels1="Line" labels2="Spline" labels3="Bar" value="0"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="auto">
                <Rhs vislevel="1" flags="1">
                  <Toggle/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="xmin">
                <Rhs vislevel="1" flags="1">
                  <Expression caption="X range minimum"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="xmax">
                <Rhs vislevel="1" flags="1">
                  <Expression caption="X range maximum"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="ymin">
                <Rhs vislevel="1" flags="1">
                  <Expression caption="Y range minimum"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="ymax">
                <Rhs vislevel="1" flags="1">
                  <Expression caption="Y range maximum"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Hist_plot_item.action Z19"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
  </Workspace>
</root>



