<?xml version="1.0"?>
<root xmlns="http://www.vips.ecs.soton.ac.uk/nip/8.5.0">
  <Workspace window_x="0" window_y="0" window_width="1280" window_height="770" filename="$HOME/GIT/nip2/test/workspaces/big_and_small_disks.ws" view="WORKSPACE_MODE_REGULAR" scale="1" offset="0" lpane_position="359" lpane_open="false" rpane_position="1274" rpane_open="false" local_defs="// private definitions for this workspace&#10;summit in mask = class Image value {&#10;&#9;value = im_compass in mask;&#10;}&#10;&#10;main = WWWW7;" name="big_and_small_disks" caption="Default empty workspace">
    <Column x="0" y="0" open="true" selected="true" sform="false" next="9" name="J" caption="input image">
      <Subcolumn vislevel="3">
        <Row popup="false" name="J3">
          <Rhs vislevel="2" flags="5">
            <iImage window_x="0" window_y="0" window_width="1674" window_height="999" image_left="1807" image_top="543" image_mag="1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="true" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Image_file &quot;$HOME/GIT/nip2/test/workspaces/gold_sample.v&quot;"/>
          </Rhs>
        </Row>
        <Row popup="false" name="J6">
          <Rhs vislevel="2" flags="5">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Image_number_format_item.Float_item.action J3"/>
          </Rhs>
        </Row>
        <Row popup="false" name="J7">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1">
              <Row name="dest">
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
              <Row name="to">
                <Rhs vislevel="1" flags="1">
                  <Option caption="Convert to" labelsn="11" labels0="Mono" labels1="sRGB" labels2="RGB16" labels3="GREY16" labels4="Lab" labels5="LabQ" labels6="LabS" labels7="LCh" labels8="XYZ" labels9="Yxy" labels10="UCS" value="1"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Colour_convert_item.sRGB_item.action J3"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="0" y="655" open="true" selected="false" sform="false" next="12" name="H" caption="(SMALL) THRESHOLD for &quot;darker in all but n directions&quot; (useable range: 0.-255.)">
      <Subcolumn vislevel="3">
        <Row popup="false" name="H2">
          <Rhs vislevel="2" flags="4">
            <iText formula="15"/>
          </Rhs>
        </Row>
        <Row popup="false" name="H11">
          <Rhs vislevel="1" flags="1">
            <iImage window_x="0" window_y="0" window_width="1280" window_height="1024" image_left="1398" image_top="944" image_mag="-2" show_status="false" show_paintbox="false" show_convert="false" show_rulers="true" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="if H10 then T57 else J3"/>
          </Rhs>
        </Row>
        <Row popup="false" name="H10">
          <Rhs vislevel="1" flags="1">
            <Toggle caption="Toggle between initial image and threshold result in H11" value="true"/>
            <Subcolumn vislevel="0">
              <Row name="caption">
                <Rhs vislevel="0" flags="4">
                  <iText formula="&quot;Toggle between initial image and threshold result in H11&quot;"/>
                </Rhs>
              </Row>
              <Row name="value">
                <Rhs vislevel="0" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="super">
                <Rhs vislevel="1" flags="4">
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Widget_toggle_item.action"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="6106" y="0" open="true" selected="false" sform="false" next="60" name="T" caption="(SMALL) disk average minus ranked average over each ring octant">
      <Subcolumn vislevel="3">
        <Row popup="false" name="T1">
          <Rhs vislevel="1" flags="4">
            <iText formula="O18"/>
          </Rhs>
        </Row>
        <Row popup="false" name="T4">
          <Rhs vislevel="4" flags="7">
            <iImage window_x="0" window_y="25" window_width="1280" window_height="727" image_left="1336" image_top="604" image_mag="1" show_status="true" show_paintbox="false" show_convert="true" show_rulers="true" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="2">
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
              <Row name="select">
                <Rhs vislevel="0" flags="4">
                  <iText formula="P1"/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Filter_rank_item.Image_rank_item.action T1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="T35">
          <Rhs vislevel="2" flags="5">
            <iText formula="Q21"/>
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
          </Rhs>
        </Row>
        <Row popup="false" name="T39">
          <Rhs vislevel="2" flags="5">
            <iImage window_x="528" window_y="25" window_width="750" window_height="722" image_left="2218" image_top="2185" image_mag="1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="true" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="(T4-T35)"/>
          </Rhs>
        </Row>
        <Row popup="false" name="T45">
          <Rhs vislevel="2" flags="5">
            <iImage window_x="0" window_y="0" window_width="1280" window_height="1024" image_left="2011" image_top="1392" image_mag="1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="true" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="T39 &gt; H2"/>
          </Rhs>
        </Row>
        <Row popup="false" name="T59">
          <Rhs vislevel="2" flags="5">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="T45 &amp; ~AAA5"/>
          </Rhs>
        </Row>
        <Row popup="false" name="T56">
          <Rhs vislevel="2" flags="5">
            <iImage window_x="0" window_y="0" window_width="1280" window_height="1024" image_left="2011" image_top="472" image_mag="1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="true" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Filter_morphology_item.Dilate_item.Dilate4_item.action T59"/>
          </Rhs>
        </Row>
        <Row popup="false" name="T57">
          <Rhs vislevel="2" flags="5">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Filter_morphology_item.Dilate_item.Dilate8_item.action T56"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="4929" y="0" open="true" selected="false" sform="false" next="24" name="N" caption="(SMALL) ring octants convolution matrices">
      <Subcolumn vislevel="3">
        <Row popup="false" name="N1">
          <Rhs vislevel="0" flags="4">
            <Matrix/>
            <Subcolumn vislevel="0"/>
            <iText formula="Matrix_to_matrix_item.action KB25"/>
          </Rhs>
        </Row>
        <Row popup="false" name="N2">
          <Rhs vislevel="1" flags="4">
            <iText formula="sum N1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="N3">
          <Rhs vislevel="0" flags="4">
            <Matrix/>
            <Subcolumn vislevel="0"/>
            <iText formula="(int) ( (65535/N2) * N1 )"/>
          </Rhs>
        </Row>
        <Row popup="false" name="N17">
          <Rhs vislevel="1" flags="4">
            <iText formula="sum N3"/>
          </Rhs>
        </Row>
        <Row popup="false" name="N15">
          <Rhs vislevel="0" flags="4">
            <Matrix/>
            <Subcolumn vislevel="0"/>
            <iText formula="Matrix_con N17 0 N3.value"/>
          </Rhs>
        </Row>
        <Row popup="false" name="N4">
          <Rhs vislevel="0" flags="4">
            <iText formula="take 8 (iterate rot45 N15)"/>
          </Rhs>
        </Row>
        <Row popup="false" name="N20">
          <Rhs vislevel="0" flags="4">
            <Matrix/>
            <Subcolumn vislevel="0"/>
            <iText formula="Matrix_to_matrix_item.action KB26"/>
          </Rhs>
        </Row>
        <Row popup="false" name="N21">
          <Rhs vislevel="0" flags="4">
            <Matrix/>
            <Subcolumn vislevel="0"/>
            <iText formula="(int) ( (65535/N2) * N20 )"/>
          </Rhs>
        </Row>
        <Row popup="false" name="N22">
          <Rhs vislevel="0" flags="4">
            <Matrix/>
            <Subcolumn vislevel="0"/>
            <iText formula="Matrix_con N17 0 N21.value"/>
          </Rhs>
        </Row>
        <Row popup="false" name="N23">
          <Rhs vislevel="1" flags="4">
            <iText formula="take 8 (iterate rot45 N22)"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="5493" y="0" open="true" selected="false" sform="false" next="19" name="O" caption="(SMALL) ring convolutions">
      <Subcolumn vislevel="3">
        <Row popup="false" name="O1">
          <Rhs vislevel="2" flags="5">
            <iImage window_x="0" window_y="25" window_width="750" window_height="722" image_left="350" image_top="307" image_mag="1" show_status="false" show_paintbox="false" show_convert="true" show_rulers="true" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="J6"/>
          </Rhs>
        </Row>
        <Row popup="false" name="O9">
          <Rhs vislevel="1" flags="4">
            <iText formula="N4"/>
          </Rhs>
        </Row>
        <Row popup="false" name="O11">
          <Rhs vislevel="1" flags="4">
            <iText formula="map (converse conv O1) O9"/>
          </Rhs>
        </Row>
        <Row popup="false" name="O15">
          <Rhs vislevel="1" flags="4">
            <iText formula="N23"/>
          </Rhs>
        </Row>
        <Row popup="false" name="O16">
          <Rhs vislevel="1" flags="4">
            <iText formula="map (converse conv O1) O15"/>
          </Rhs>
        </Row>
        <Row popup="false" name="O17">
          <Rhs vislevel="1" flags="4">
            <iText formula="[ O11, O16 ]"/>
          </Rhs>
        </Row>
        <Row popup="false" name="O18">
          <Rhs vislevel="1" flags="4">
            <iText formula="Math_list_item.Concat_item.action O17"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="3647" y="0" open="true" selected="false" sform="false" next="15" name="D" caption="(SMALL) octant mask">
      <Subcolumn vislevel="3">
        <Row popup="false" name="D14">
          <Rhs vislevel="1" flags="4">
            <iText formula="OB1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="D1">
          <Rhs vislevel="3" flags="4">
            <iText formula="2*D14-1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="D2">
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
                  <Subcolumn vislevel="0">
                    <Row name="caption">
                      <Rhs vislevel="0" flags="4">
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="expr">
                      <Rhs vislevel="0" flags="4">
                        <iText formula="D1"/>
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
                  <Expression caption="Image height (pixels)"/>
                  <Subcolumn vislevel="0">
                    <Row name="caption">
                      <Rhs vislevel="0" flags="4">
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="expr">
                      <Rhs vislevel="0" flags="4">
                        <iText formula="D1"/>
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
            <iText formula="Pattern_images_item.Xy_item.action"/>
          </Rhs>
        </Row>
        <Row popup="false" name="D3">
          <Rhs vislevel="2" flags="5">
            <iImage window_x="173" window_y="308" window_width="665" window_height="627" image_left="78" image_top="62" image_mag="4" show_status="true" show_paintbox="false" show_convert="true" show_rulers="true" scale="9.7957272581564077" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="D2 - (D1 + 1) / 2"/>
          </Rhs>
        </Row>
        <Row popup="false" name="D4">
          <Rhs vislevel="2" flags="5">
            <iImage window_x="0" window_y="0" window_width="1280" window_height="1024" image_left="19" image_top="15" image_mag="32" show_status="false" show_paintbox="false" show_convert="false" show_rulers="true" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0">
              <Row name="value">
                <Rhs vislevel="0" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="super">
                <Rhs vislevel="1" flags="4">
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="width">
                <Rhs vislevel="1" flags="4">
                  <iText formula="D1"/>
                </Rhs>
              </Row>
              <Row name="height">
                <Rhs vislevel="1" flags="4">
                  <iText formula="D1"/>
                </Rhs>
              </Row>
              <Row name="bands">
                <Rhs vislevel="1" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="format">
                <Rhs vislevel="1" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="bits">
                <Rhs vislevel="1" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="coding">
                <Rhs vislevel="1" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="type">
                <Rhs vislevel="1" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="xres">
                <Rhs vislevel="1" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="yres">
                <Rhs vislevel="1" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="xoffset">
                <Rhs vislevel="1" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="yoffset">
                <Rhs vislevel="1" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="filename">
                <Rhs vislevel="1" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="rect">
                <Rhs vislevel="1" flags="4">
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="D3?0 - D3?1 &gt; 0"/>
          </Rhs>
        </Row>
        <Row popup="false" name="D5">
          <Rhs vislevel="2" flags="5">
            <iImage window_x="0" window_y="0" window_width="1280" window_height="1024" image_left="19" image_top="15" image_mag="32" show_status="false" show_paintbox="false" show_convert="false" show_rulers="true" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0">
              <Row name="value">
                <Rhs vislevel="0" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="super">
                <Rhs vislevel="1" flags="4">
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="width">
                <Rhs vislevel="1" flags="4">
                  <iText formula="D1"/>
                </Rhs>
              </Row>
              <Row name="height">
                <Rhs vislevel="1" flags="4">
                  <iText formula="D1"/>
                </Rhs>
              </Row>
              <Row name="bands">
                <Rhs vislevel="1" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="format">
                <Rhs vislevel="1" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="bits">
                <Rhs vislevel="1" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="coding">
                <Rhs vislevel="1" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="type">
                <Rhs vislevel="1" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="xres">
                <Rhs vislevel="1" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="yres">
                <Rhs vislevel="1" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="xoffset">
                <Rhs vislevel="1" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="yoffset">
                <Rhs vislevel="1" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="filename">
                <Rhs vislevel="1" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="rect">
                <Rhs vislevel="1" flags="4">
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="D3?0 &lt; 0"/>
          </Rhs>
        </Row>
        <Row popup="false" name="D6">
          <Rhs vislevel="1" flags="1">
            <iImage window_x="0" window_y="0" window_width="1280" window_height="1024" image_left="38" image_top="27" image_mag="16" show_status="true" show_paintbox="false" show_convert="true" show_rulers="true" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="D4 &amp; D5"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="6771" y="0" open="true" selected="false" sform="false" next="12" name="R" caption="(SMALL) centroids (computed with an indexed histogram)">
      <Subcolumn vislevel="3">
        <Row popup="false" name="R2">
          <Rhs vislevel="0" flags="4">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="C1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="R1">
          <Rhs vislevel="2" flags="5">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Image_select_item.Segment_item.action ~R2"/>
          </Rhs>
        </Row>
        <Row popup="false" name="R4">
          <Rhs vislevel="0" flags="4">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Image_number_format_item.U16_item.action R1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="R3">
          <Rhs vislevel="2" flags="5">
            <iImage window_x="0" window_y="23" window_width="750" window_height="750" image_left="347" image_top="335" image_mag="1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="true" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0">
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
                  <Subcolumn vislevel="0">
                    <Row name="caption">
                      <Rhs vislevel="0" flags="4">
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="expr">
                      <Rhs vislevel="0" flags="4">
                        <iText formula="R2.width"/>
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
                  <Expression caption="Image height (pixels)"/>
                  <Subcolumn vislevel="0">
                    <Row name="caption">
                      <Rhs vislevel="0" flags="4">
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="expr">
                      <Rhs vislevel="0" flags="4">
                        <iText formula="R2.height"/>
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
            <iText formula="Pattern_images_item.Xy_item.action"/>
          </Rhs>
        </Row>
        <Row popup="false" name="R5">
          <Rhs vislevel="4" flags="7">
            <Plot plot_left="0" plot_top="0" plot_mag="100" show_status="false"/>
            <Subcolumn vislevel="2"/>
            <iText formula="Hist_find_item.Indexed_item.action R4 R3"/>
          </Rhs>
        </Row>
        <Row popup="false" name="R6">
          <Rhs vislevel="3" flags="7">
            <Plot plot_left="0" plot_top="0" plot_mag="100" show_status="false"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Hist_find_item.Oned_item.action R4"/>
          </Rhs>
        </Row>
        <Row popup="false" name="R7">
          <Rhs vislevel="3" flags="7">
            <Plot window_x="1412" window_y="667" window_width="500" window_height="500" plot_left="231" plot_top="200" plot_mag="100" show_status="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="R5/R6 + .5"/>
          </Rhs>
        </Row>
        <Row popup="false" name="R8">
          <Rhs vislevel="2" flags="5">
            <Matrix/>
            <Subcolumn vislevel="0"/>
            <iText formula="Matrix_to_matrix_item.action R7"/>
          </Rhs>
        </Row>
        <Row popup="false" name="R11">
          <Rhs vislevel="1" flags="4">
            <iText formula="&quot;the first two columns are 'unset' and background, delete them&quot;"/>
          </Rhs>
        </Row>
        <Row popup="false" name="R10">
          <Rhs vislevel="3" flags="7">
            <Matrix/>
            <Subcolumn vislevel="1">
              <Row name="x">
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
              <Row name="first">
                <Rhs vislevel="1" flags="1">
                  <Expression caption="Delete from column"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="number">
                <Rhs vislevel="1" flags="1">
                  <Expression caption="Delete this many columns"/>
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
            <iText formula="Matrix_delete_item.Columns_item.action R8"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="7261" y="0" open="true" selected="false" sform="false" next="29" name="G" caption="(SMALL) draw red crosses at centroid locations">
      <Subcolumn vislevel="3">
        <Row popup="false" name="G1">
          <Rhs vislevel="0" flags="4">
            <Matrix/>
            <Subcolumn vislevel="0"/>
            <iText formula="R10"/>
          </Rhs>
        </Row>
        <Row popup="false" name="G2">
          <Rhs vislevel="0" flags="4">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="J3"/>
          </Rhs>
        </Row>
        <Row popup="false" name="G3">
          <Rhs vislevel="1" flags="4">
            <iText formula="&quot; &quot;"/>
          </Rhs>
        </Row>
        <Row popup="false" name="G4">
          <Rhs vislevel="1" flags="4">
            <iText formula="&quot;draw this on the image:&quot;"/>
          </Rhs>
        </Row>
        <Row popup="false" name="G5">
          <Rhs vislevel="2" flags="5">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0">
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
                  <Subcolumn vislevel="0">
                    <Row name="caption">
                      <Rhs vislevel="0" flags="4">
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="expr">
                      <Rhs vislevel="0" flags="4">
                        <iText formula="1"/>
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
                  <Expression caption="Image height (pixels)"/>
                  <Subcolumn vislevel="0">
                    <Row name="caption">
                      <Rhs vislevel="0" flags="4">
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="expr">
                      <Rhs vislevel="0" flags="4">
                        <iText formula="1"/>
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
              <Row name="nbands">
                <Rhs vislevel="1" flags="1">
                  <Expression caption="Image bands"/>
                  <Subcolumn vislevel="0">
                    <Row name="caption">
                      <Rhs vislevel="0" flags="4">
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="expr">
                      <Rhs vislevel="0" flags="4">
                        <iText formula="1"/>
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
              <Row name="format_option">
                <Rhs vislevel="1" flags="1">
                  <Option/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="type_option">
                <Rhs vislevel="1" flags="1">
                  <Option/>
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
                        <iText formula="255"/>
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
        <Row popup="false" name="G17">
          <Rhs vislevel="2" flags="5">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0">
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
                  <Subcolumn vislevel="0">
                    <Row name="caption">
                      <Rhs vislevel="0" flags="4">
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="expr">
                      <Rhs vislevel="0" flags="4">
                        <iText formula="G2.width"/>
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
                  <Expression caption="Image height (pixels)"/>
                  <Subcolumn vislevel="0">
                    <Row name="caption">
                      <Rhs vislevel="0" flags="4">
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="expr">
                      <Rhs vislevel="0" flags="4">
                        <iText formula="G2.height"/>
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
              <Row name="nbands">
                <Rhs vislevel="1" flags="1">
                  <Expression caption="Image bands"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="format_option">
                <Rhs vislevel="1" flags="1">
                  <Option/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="type_option">
                <Rhs vislevel="1" flags="1">
                  <Option/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="pixel">
                <Rhs vislevel="1" flags="1">
                  <Expression caption="Pixel value"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Image_new_item.Image_black_item.action"/>
          </Rhs>
        </Row>
        <Row popup="false" name="G9">
          <Rhs vislevel="1" flags="4">
            <iText formula="map (converse subtract (G5.width / 2)) G1.value?0"/>
          </Rhs>
        </Row>
        <Row popup="false" name="G10">
          <Rhs vislevel="1" flags="4">
            <iText formula="map (converse subtract (G5.height / 2)) G1.value?1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="G11">
          <Rhs vislevel="1" flags="4">
            <iText formula="im_insertset G17.value G5.value G9 G10"/>
          </Rhs>
        </Row>
        <Row popup="false" name="G12">
          <Rhs vislevel="2" flags="5">
            <iImage window_x="142" window_y="165" window_width="750" window_height="750" image_left="1043" image_top="310" image_mag="1" show_status="true" show_paintbox="false" show_convert="true" show_rulers="false" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Image G11"/>
          </Rhs>
        </Row>
        <Row popup="false" name="G27">
          <Rhs vislevel="2" flags="5">
            <iImage window_x="0" window_y="1" window_width="437" window_height="349" image_left="203" image_top="118" image_mag="1" show_status="true" show_paintbox="false" show_convert="true" show_rulers="false" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Filter_morphology_item.Dilate_item.Dilate4_item.action G12"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="7743" y="0" open="true" selected="false" sform="false" next="10" name="I" caption="(SMALL) plot 3x3 crosses on original image">
      <Subcolumn vislevel="3">
        <Row popup="false" name="I1">
          <Rhs vislevel="0" flags="4">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="CB5"/>
          </Rhs>
        </Row>
        <Row popup="false" name="I2">
          <Rhs vislevel="0" flags="4">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="G27"/>
          </Rhs>
        </Row>
        <Row popup="false" name="I3">
          <Rhs vislevel="1" flags="4">
            <iText formula="&quot; &quot;"/>
          </Rhs>
        </Row>
        <Row popup="false" name="I5">
          <Rhs vislevel="2" flags="5">
            <iImage window_x="264" window_y="1" window_width="750" window_height="714" image_left="368" image_top="300" image_mag="1" show_status="true" show_paintbox="false" show_convert="true" show_rulers="false" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="if I2 then Vector [255,0,0]  else I1"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="774" y="66" open="true" selected="false" sform="false" next="7" name="E" caption="FINAL IMAGE RESULT with small nanoparticles tagged with a 3x3 red cross and big disks tagged with a green cross">
      <Subcolumn vislevel="3">
        <Row popup="false" name="E1">
          <Rhs vislevel="1" flags="1">
            <iImage window_x="0" window_y="25" window_width="1278" window_height="722" image_left="622" image_top="332" image_mag="1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="true" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="if E3 then I5 else J7"/>
          </Rhs>
        </Row>
        <Row popup="false" name="E3">
          <Rhs vislevel="1" flags="1">
            <Toggle caption="Toggle the visibility of detected nanoparticles on or off in E1" value="true"/>
            <Subcolumn vislevel="0">
              <Row name="caption">
                <Rhs vislevel="0" flags="4">
                  <iText formula="&quot;Toggle the visibility of detected nanoparticles on or off in E1&quot;"/>
                </Rhs>
              </Row>
              <Row name="value">
                <Rhs vislevel="0" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="super">
                <Rhs vislevel="2" flags="6">
                  <Subcolumn vislevel="1"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Widget_toggle_item.action"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="0" y="1900" open="true" selected="false" sform="false" next="2" name="L" caption="(SMALL) number of regions">
      <Subcolumn vislevel="3">
        <Row popup="false" name="L1">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="R1"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="2637" y="0" open="true" selected="false" sform="false" next="3" name="B" caption="(SMALL) &quot;dark enough&quot; test">
      <Subcolumn vislevel="3">
        <Row popup="false" name="B1">
          <Rhs vislevel="3" flags="7">
            <iImage window_x="0" window_y="0" window_width="750" window_height="750" image_left="1746" image_top="335" image_mag="1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="true" scale="1" offset="0" falsecolour="false" type="true"/>
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
              <Row name="type">
                <Rhs vislevel="1" flags="1">
                  <Option/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="r">
                <Rhs vislevel="1" flags="1">
                  <Slider caption="Radius" from="1" to="100" value="3"/>
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
                  <Option caption="Precision" labelsn="2" labels0="Int" labels1="Float" value="1"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="layers">
                <Rhs vislevel="1" flags="1">
                  <Slider/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Filter_conv_item.Custom_blur_item.action J6"/>
          </Rhs>
        </Row>
        <Row popup="false" name="B2">
          <Rhs vislevel="2" flags="5">
            <iImage window_x="0" window_y="0" window_width="1674" window_height="999" image_left="1808" image_top="1429" image_mag="1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="true" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="B1 &lt; F1"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="0" y="1615" open="true" selected="false" sform="false" next="5" name="C" caption="(SMALL) darker in most directions combined with dark at the center">
      <Subcolumn vislevel="3">
        <Row popup="false" name="C1">
          <Rhs vislevel="2" flags="5">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="T57 &amp; B2"/>
          </Rhs>
        </Row>
        <Row popup="false" name="C4">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="if C2 then C1 else J3"/>
          </Rhs>
        </Row>
        <Row popup="false" name="C2">
          <Rhs vislevel="1" flags="1">
            <Toggle caption="Toggle between initial image and combined threshold results in C3" value="true"/>
            <Subcolumn vislevel="0">
              <Row name="caption">
                <Rhs vislevel="0" flags="4">
                  <iText formula="&quot;Toggle between initial image and combined threshold results in C3&quot;"/>
                </Rhs>
              </Row>
              <Row name="value">
                <Rhs vislevel="0" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="super">
                <Rhs vislevel="1" flags="4">
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Widget_toggle_item.action"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="0" y="1201" open="true" selected="false" sform="false" next="4" name="F" caption="(SMALL) THRESHOLD for &quot;dark enough&quot; test (useable range: 0.-255.)">
      <Subcolumn vislevel="3">
        <Row popup="false" name="F1">
          <Rhs vislevel="1" flags="4">
            <iText formula="145"/>
          </Rhs>
        </Row>
        <Row popup="false" name="F3">
          <Rhs vislevel="1" flags="1">
            <iImage window_x="261" window_y="74" window_width="750" window_height="750" image_left="1746" image_top="2015" image_mag="1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="true" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="if F2 then B2 else J3"/>
          </Rhs>
        </Row>
        <Row popup="false" name="F2">
          <Rhs vislevel="1" flags="1">
            <Toggle caption="Toggle between initial image and threshold result in F3" value="true"/>
            <Subcolumn vislevel="0">
              <Row name="caption">
                <Rhs vislevel="0" flags="4">
                  <iText formula="&quot;Toggle between initial image and threshold result in F3&quot;"/>
                </Rhs>
              </Row>
              <Row name="value">
                <Rhs vislevel="0" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="super">
                <Rhs vislevel="1" flags="4">
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Widget_toggle_item.action"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="0" y="523" open="true" selected="false" sform="false" next="3" name="P" caption="(SMALL) NUMBER of directions to ignore in the &quot;darker in all but n directions&quot; test (useable range: 0-15)">
      <Subcolumn vislevel="3">
        <Row popup="false" name="P2">
          <Rhs vislevel="1" flags="4">
            <iText formula="&quot;There are 16 possible ranked directions because we symmetrized the family of sharp octants&quot;"/>
          </Rhs>
        </Row>
        <Row popup="false" name="P1">
          <Rhs vislevel="1" flags="4">
            <iText formula="2"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="0" y="994" open="true" selected="false" sform="false" next="11" name="U" caption="(BIG) THRESHOLD for &quot;darker in all but n directions&quot; (useable range: 0.-255.)">
      <Subcolumn vislevel="3">
        <Row popup="false" name="U1">
          <Rhs vislevel="2" flags="4">
            <iText formula="25"/>
          </Rhs>
        </Row>
        <Row popup="false" name="U10">
          <Rhs vislevel="1" flags="1">
            <iImage window_x="0" window_y="0" window_width="1280" window_height="1024" image_left="612" image_top="1400" image_mag="1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="true" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="if U9 then W7 else J3"/>
          </Rhs>
        </Row>
        <Row popup="false" name="U9">
          <Rhs vislevel="1" flags="1">
            <Toggle caption="Toggle between initial image and threshold result in U10" value="true"/>
            <Subcolumn vislevel="0">
              <Row name="caption">
                <Rhs vislevel="0" flags="4">
                  <iText formula="&quot;Toggle between initial image and threshold result in U10&quot;"/>
                </Rhs>
              </Row>
              <Row name="value">
                <Rhs vislevel="0" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="super">
                <Rhs vislevel="1" flags="4">
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Widget_toggle_item.action"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="11548" y="0" open="true" selected="false" sform="false" next="59" name="W" caption="(BIG) disk average minus the ranked wedge average">
      <Subcolumn vislevel="3">
        <Row popup="false" name="W1">
          <Rhs vislevel="1" flags="4">
            <iText formula="Y3"/>
          </Rhs>
        </Row>
        <Row popup="false" name="W2">
          <Rhs vislevel="4" flags="7">
            <iImage window_x="0" window_y="1" window_width="1680" window_height="1050" image_left="814" image_top="450" image_mag="1" show_status="true" show_paintbox="false" show_convert="true" show_rulers="true" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="2">
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
              <Row name="select">
                <Rhs vislevel="0" flags="4">
                  <iText formula="JB1"/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Filter_rank_item.Image_rank_item.action W1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="W3">
          <Rhs vislevel="2" flags="5">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="K12"/>
          </Rhs>
        </Row>
        <Row popup="false" name="W4">
          <Rhs vislevel="2" flags="5">
            <iImage window_x="528" window_y="25" window_width="750" window_height="722" image_left="2218" image_top="2185" image_mag="1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="true" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="(W2-W3)"/>
          </Rhs>
        </Row>
        <Row popup="false" name="W5">
          <Rhs vislevel="2" flags="5">
            <iImage window_x="0" window_y="0" window_width="1280" window_height="1024" image_left="2011" image_top="1392" image_mag="1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="true" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="W4 &gt; U1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="W6">
          <Rhs vislevel="2" flags="5">
            <iImage window_x="0" window_y="0" window_width="1280" window_height="1024" image_left="2011" image_top="472" image_mag="1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="true" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Filter_morphology_item.Dilate_item.Dilate4_item.action W5"/>
          </Rhs>
        </Row>
        <Row popup="false" name="W7">
          <Rhs vislevel="2" flags="5">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Filter_morphology_item.Dilate_item.Dilate8_item.action W6"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="10383" y="0" open="true" selected="false" sform="false" next="20" name="X" caption="(BIG) ring octants masks">
      <Subcolumn vislevel="3">
        <Row popup="false" name="X1">
          <Rhs vislevel="0" flags="4">
            <Matrix/>
            <Subcolumn vislevel="0"/>
            <iText formula="Matrix_to_matrix_item.action SB10"/>
          </Rhs>
        </Row>
        <Row popup="false" name="X2">
          <Rhs vislevel="1" flags="4">
            <iText formula="sum X1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="X3">
          <Rhs vislevel="0" flags="4">
            <Matrix/>
            <Subcolumn vislevel="0"/>
            <iText formula="(int) ( (65535/X2) * X1 )"/>
          </Rhs>
        </Row>
        <Row popup="false" name="X4">
          <Rhs vislevel="1" flags="4">
            <iText formula="sum X3"/>
          </Rhs>
        </Row>
        <Row popup="false" name="X5">
          <Rhs vislevel="0" flags="4">
            <Matrix/>
            <Subcolumn vislevel="0"/>
            <iText formula="Matrix_con X4 0 X3.value"/>
          </Rhs>
        </Row>
        <Row popup="false" name="X6">
          <Rhs vislevel="0" flags="4">
            <iText formula="take 8 (iterate rot45 X5)"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="10946" y="0" open="true" selected="false" sform="false" next="15" name="Y" caption="(BIG) ring wedge averages">
      <Subcolumn vislevel="3">
        <Row popup="false" name="Y1">
          <Rhs vislevel="2" flags="5">
            <iImage window_x="0" window_y="0" window_width="750" window_height="750" image_left="349" image_top="319" image_mag="1" show_status="false" show_paintbox="false" show_convert="true" show_rulers="true" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="J6"/>
          </Rhs>
        </Row>
        <Row popup="false" name="Y2">
          <Rhs vislevel="1" flags="4">
            <iText formula="X6"/>
          </Rhs>
        </Row>
        <Row popup="false" name="Y3">
          <Rhs vislevel="1" flags="4">
            <iText formula="map (converse conv Y1) Y2"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="9105" y="0" open="true" selected="false" sform="false" next="15" name="Z" caption="(BIG) octant masks">
      <Subcolumn vislevel="3">
        <Row popup="false" name="Z13">
          <Rhs vislevel="1" flags="4">
            <iText formula="QB1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="Z14">
          <Rhs vislevel="1" flags="4">
            <iText formula="2*Z13-1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="Z2">
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
                  <Subcolumn vislevel="0">
                    <Row name="caption">
                      <Rhs vislevel="0" flags="4">
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="expr">
                      <Rhs vislevel="0" flags="4">
                        <iText formula="Z14"/>
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
                  <Expression caption="Image height (pixels)"/>
                  <Subcolumn vislevel="0">
                    <Row name="caption">
                      <Rhs vislevel="0" flags="4">
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="expr">
                      <Rhs vislevel="0" flags="4">
                        <iText formula="Z14"/>
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
            <iText formula="Pattern_images_item.Xy_item.action"/>
          </Rhs>
        </Row>
        <Row popup="false" name="Z3">
          <Rhs vislevel="2" flags="5">
            <iImage window_x="173" window_y="308" window_width="665" window_height="627" image_left="78" image_top="62" image_mag="4" show_status="true" show_paintbox="false" show_convert="true" show_rulers="true" scale="9.7957272581564077" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Z2 - (Z14 + 1) / 2"/>
          </Rhs>
        </Row>
        <Row popup="false" name="Z4">
          <Rhs vislevel="2" flags="5">
            <iImage window_x="0" window_y="0" window_width="1280" window_height="1024" image_left="19" image_top="15" image_mag="32" show_status="false" show_paintbox="false" show_convert="false" show_rulers="true" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Z3?0 - Z3?1 &gt;= 0"/>
          </Rhs>
        </Row>
        <Row popup="false" name="Z5">
          <Rhs vislevel="2" flags="5">
            <iImage window_x="0" window_y="0" window_width="1280" window_height="1024" image_left="19" image_top="15" image_mag="32" show_status="false" show_paintbox="false" show_convert="false" show_rulers="true" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0">
              <Row name="value">
                <Rhs vislevel="0" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="super">
                <Rhs vislevel="1" flags="4">
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="width">
                <Rhs vislevel="1" flags="4">
                  <iText formula="Z14"/>
                </Rhs>
              </Row>
              <Row name="height">
                <Rhs vislevel="1" flags="4">
                  <iText formula="Z14"/>
                </Rhs>
              </Row>
              <Row name="bands">
                <Rhs vislevel="1" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="format">
                <Rhs vislevel="1" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="bits">
                <Rhs vislevel="1" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="coding">
                <Rhs vislevel="1" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="type">
                <Rhs vislevel="1" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="xres">
                <Rhs vislevel="1" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="yres">
                <Rhs vislevel="1" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="xoffset">
                <Rhs vislevel="1" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="yoffset">
                <Rhs vislevel="1" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="filename">
                <Rhs vislevel="1" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="rect">
                <Rhs vislevel="1" flags="4">
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Z3?0 &lt; 0"/>
          </Rhs>
        </Row>
        <Row popup="false" name="Z6">
          <Rhs vislevel="1" flags="1">
            <iImage window_x="0" window_y="0" window_width="1280" window_height="1024" image_left="38" image_top="27" image_mag="16" show_status="true" show_paintbox="false" show_convert="true" show_rulers="true" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Z4 &amp; Z5"/>
          </Rhs>
        </Row>
        <Row popup="false" name="Z7">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Z3?0 == -1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="Z8">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Z3?0 == Z3?1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="Z9">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Z7 | Z8"/>
          </Rhs>
        </Row>
        <Row popup="false" name="Z10">
          <Rhs vislevel="2" flags="5">
            <iImage window_x="155" window_y="63" window_width="634" window_height="651" image_left="18" image_top="16" image_mag="16" show_status="true" show_paintbox="false" show_convert="true" show_rulers="true" scale="128" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="if Z6 &amp; Z9 then 1 else if Z6 then 2 else 0"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="12210" y="0" open="true" selected="false" sform="false" next="13" name="AB" caption="(BIG) region centroids (computed with indexed histogram)">
      <Subcolumn vislevel="3">
        <Row popup="false" name="AB1">
          <Rhs vislevel="0" flags="4">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="AAA3"/>
          </Rhs>
        </Row>
        <Row popup="false" name="AB2">
          <Rhs vislevel="3" flags="7">
            <iImage window_x="0" window_y="1" window_width="750" window_height="750" image_left="175" image_top="154" image_mag="2" show_status="true" show_paintbox="false" show_convert="true" show_rulers="true" scale="137.81365748984248" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Image_select_item.Segment_item.action ~AB1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="AB3">
          <Rhs vislevel="0" flags="4">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Image_number_format_item.U16_item.action AB2"/>
          </Rhs>
        </Row>
        <Row popup="false" name="AB4">
          <Rhs vislevel="2" flags="5">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0">
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
                  <Subcolumn vislevel="0">
                    <Row name="caption">
                      <Rhs vislevel="0" flags="4">
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="expr">
                      <Rhs vislevel="0" flags="4">
                        <iText formula="AB1.width"/>
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
                  <Expression caption="Image height (pixels)"/>
                  <Subcolumn vislevel="0">
                    <Row name="caption">
                      <Rhs vislevel="0" flags="4">
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="expr">
                      <Rhs vislevel="0" flags="4">
                        <iText formula="AB1.height"/>
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
            <iText formula="Pattern_images_item.Xy_item.action"/>
          </Rhs>
        </Row>
        <Row popup="false" name="AB5">
          <Rhs vislevel="4" flags="7">
            <Plot plot_left="0" plot_top="0" plot_mag="100" show_status="false"/>
            <Subcolumn vislevel="2"/>
            <iText formula="Hist_find_item.Indexed_item.action AB3 AB4"/>
          </Rhs>
        </Row>
        <Row popup="false" name="AB6">
          <Rhs vislevel="3" flags="7">
            <Plot plot_left="0" plot_top="0" plot_mag="100" show_status="false"/>
            <Subcolumn vislevel="1"/>
            <iText formula="Hist_find_item.Oned_item.action AB3"/>
          </Rhs>
        </Row>
        <Row popup="false" name="AB7">
          <Rhs vislevel="3" flags="7">
            <Plot window_x="1412" window_y="667" window_width="500" window_height="500" plot_left="231" plot_top="200" plot_mag="100" show_status="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="AB5/AB6 + .5"/>
          </Rhs>
        </Row>
        <Row popup="false" name="AB8">
          <Rhs vislevel="1" flags="1">
            <Matrix/>
            <Subcolumn vislevel="0"/>
            <iText formula="Matrix_to_matrix_item.action AB7"/>
          </Rhs>
        </Row>
        <Row popup="false" name="AB12">
          <Rhs vislevel="1" flags="4">
            <iText formula="&quot;the first two columns are 'unset' and background, delete them&quot;"/>
          </Rhs>
        </Row>
        <Row popup="false" name="AB11">
          <Rhs vislevel="3" flags="7">
            <Matrix/>
            <Subcolumn vislevel="1">
              <Row name="x">
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
              <Row name="first">
                <Rhs vislevel="1" flags="1">
                  <Expression caption="Delete from column"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="number">
                <Rhs vislevel="1" flags="1">
                  <Expression caption="Delete this many columns"/>
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
            <iText formula="Matrix_delete_item.Columns_item.action AB8"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="12849" y="0" open="true" selected="false" sform="false" next="29" name="BB" caption="(BIG) put 3x3 crosses centered at centroids">
      <Subcolumn vislevel="3">
        <Row popup="false" name="BB1">
          <Rhs vislevel="0" flags="4">
            <Matrix/>
            <Subcolumn vislevel="0"/>
            <iText formula="AB11"/>
          </Rhs>
        </Row>
        <Row popup="false" name="BB2">
          <Rhs vislevel="0" flags="4">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="J3"/>
          </Rhs>
        </Row>
        <Row popup="false" name="BB3">
          <Rhs vislevel="1" flags="4">
            <iText formula="&quot; &quot;"/>
          </Rhs>
        </Row>
        <Row popup="false" name="BB4">
          <Rhs vislevel="1" flags="4">
            <iText formula="&quot;draw this on the image:&quot;"/>
          </Rhs>
        </Row>
        <Row popup="false" name="BB5">
          <Rhs vislevel="2" flags="5">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0">
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
                  <Subcolumn vislevel="0">
                    <Row name="caption">
                      <Rhs vislevel="0" flags="4">
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="expr">
                      <Rhs vislevel="0" flags="4">
                        <iText formula="1"/>
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
                  <Expression caption="Image height (pixels)"/>
                  <Subcolumn vislevel="0">
                    <Row name="caption">
                      <Rhs vislevel="0" flags="4">
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="expr">
                      <Rhs vislevel="0" flags="4">
                        <iText formula="1"/>
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
              <Row name="nbands">
                <Rhs vislevel="1" flags="1">
                  <Expression caption="Image bands"/>
                  <Subcolumn vislevel="0">
                    <Row name="caption">
                      <Rhs vislevel="0" flags="4">
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="expr">
                      <Rhs vislevel="0" flags="4">
                        <iText formula="1"/>
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
              <Row name="format_option">
                <Rhs vislevel="1" flags="1">
                  <Option/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="type_option">
                <Rhs vislevel="1" flags="1">
                  <Option/>
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
                        <iText formula="255"/>
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
        <Row popup="false" name="BB6">
          <Rhs vislevel="2" flags="5">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0">
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
                  <Subcolumn vislevel="0">
                    <Row name="caption">
                      <Rhs vislevel="0" flags="4">
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="expr">
                      <Rhs vislevel="0" flags="4">
                        <iText formula="BB2.width"/>
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
                  <Expression caption="Image height (pixels)"/>
                  <Subcolumn vislevel="0">
                    <Row name="caption">
                      <Rhs vislevel="0" flags="4">
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="expr">
                      <Rhs vislevel="0" flags="4">
                        <iText formula="BB2.height"/>
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
              <Row name="nbands">
                <Rhs vislevel="1" flags="1">
                  <Expression caption="Image bands"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="format_option">
                <Rhs vislevel="1" flags="1">
                  <Option/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="type_option">
                <Rhs vislevel="1" flags="1">
                  <Option/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="pixel">
                <Rhs vislevel="1" flags="1">
                  <Expression caption="Pixel value"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Image_new_item.Image_black_item.action"/>
          </Rhs>
        </Row>
        <Row popup="false" name="BB9">
          <Rhs vislevel="1" flags="4">
            <iText formula="map (converse subtract (BB5.width / 2)) BB1.value?0"/>
          </Rhs>
        </Row>
        <Row popup="false" name="BB10">
          <Rhs vislevel="1" flags="4">
            <iText formula="map (converse subtract (BB5.height / 2)) BB1.value?1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="BB11">
          <Rhs vislevel="1" flags="4">
            <iText formula="im_insertset BB6.value BB5.value BB9 BB10"/>
          </Rhs>
        </Row>
        <Row popup="false" name="BB12">
          <Rhs vislevel="2" flags="5">
            <iImage window_x="142" window_y="165" window_width="750" window_height="750" image_left="1043" image_top="310" image_mag="1" show_status="true" show_paintbox="false" show_convert="true" show_rulers="false" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Image BB11"/>
          </Rhs>
        </Row>
        <Row popup="false" name="BB13">
          <Rhs vislevel="2" flags="5">
            <iImage window_x="0" window_y="1" window_width="437" window_height="349" image_left="203" image_top="118" image_mag="1" show_status="true" show_paintbox="false" show_convert="true" show_rulers="false" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Filter_morphology_item.Dilate_item.Dilate4_item.action BB12"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="13471" y="0" open="true" selected="false" sform="false" next="10" name="CB" caption="(BIG) plot crosses on original">
      <Subcolumn vislevel="3">
        <Row popup="false" name="CB1">
          <Rhs vislevel="0" flags="4">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="J3"/>
          </Rhs>
        </Row>
        <Row popup="false" name="CB2">
          <Rhs vislevel="0" flags="4">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="BB13"/>
          </Rhs>
        </Row>
        <Row popup="false" name="CB3">
          <Rhs vislevel="1" flags="4">
            <iText formula="&quot; &quot;"/>
          </Rhs>
        </Row>
        <Row popup="false" name="CB4">
          <Rhs vislevel="2" flags="5">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Colour_convert_item.sRGB_item.action CB1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="CB5">
          <Rhs vislevel="2" flags="5">
            <iImage window_x="264" window_y="1" window_width="750" window_height="714" image_left="368" image_top="300" image_mag="1" show_status="true" show_paintbox="false" show_convert="true" show_rulers="false" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="if CB2 then Vector [0,255,0]  else CB4"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="0" y="2395" open="true" selected="false" sform="false" next="2" name="EB" caption="(BIG) number of regions">
      <Subcolumn vislevel="3">
        <Row popup="false" name="EB1">
          <Rhs vislevel="3" flags="7">
            <iImage window_x="0" window_y="0" window_width="1280" window_height="1024" image_left="1631" image_top="472" image_mag="1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="true" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1"/>
            <iText formula="AB2"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="8120" y="0" open="true" selected="false" sform="false" next="3" name="GB" caption="(BIG) &quot;dark enough&quot; test">
      <Subcolumn vislevel="3">
        <Row popup="false" name="GB1">
          <Rhs vislevel="3" flags="7">
            <iImage window_x="0" window_y="25" window_width="750" window_height="722" image_left="1746" image_top="324" image_mag="1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="true" scale="1" offset="0" falsecolour="false" type="true"/>
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
              <Row name="type">
                <Rhs vislevel="1" flags="1">
                  <Option/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="r">
                <Rhs vislevel="1" flags="1">
                  <Slider caption="Radius" from="1" to="100" value="12"/>
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
                  <Option caption="Precision" labelsn="2" labels0="Int" labels1="Float" value="1"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="layers">
                <Rhs vislevel="1" flags="1">
                  <Slider/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Filter_conv_item.Custom_blur_item.action J6"/>
          </Rhs>
        </Row>
        <Row popup="false" name="GB2">
          <Rhs vislevel="2" flags="5">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="GB1 &lt; IB1"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="0" y="2110" open="true" selected="false" sform="false" next="4" name="HB" caption="(BIG) darker in most directions combined with dark at the center">
      <Subcolumn vislevel="3">
        <Row popup="false" name="HB1">
          <Rhs vislevel="2" flags="5">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="W7 &amp; GB2"/>
          </Rhs>
        </Row>
        <Row popup="false" name="HB3">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="if HB2 then HB1 else J3"/>
          </Rhs>
        </Row>
        <Row popup="false" name="HB2">
          <Rhs vislevel="1" flags="1">
            <Toggle caption="Toggle between initial image and combined threshold result in HB3" value="true"/>
            <Subcolumn vislevel="0">
              <Row name="caption">
                <Rhs vislevel="0" flags="4">
                  <iText formula="&quot;Toggle between initial image and combined threshold result in HB3&quot;"/>
                </Rhs>
              </Row>
              <Row name="value">
                <Rhs vislevel="0" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="super">
                <Rhs vislevel="1" flags="4">
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Widget_toggle_item.action"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="0" y="1408" open="true" selected="false" sform="false" next="4" name="IB" caption="(BIG) THRESHOLD for &quot;dark enough&quot; test (useable range: 0.-255.)">
      <Subcolumn vislevel="3">
        <Row popup="false" name="IB1">
          <Rhs vislevel="1" flags="4">
            <iText formula="90"/>
          </Rhs>
        </Row>
        <Row popup="false" name="IB3">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="if IB2 then GB2 else J3"/>
          </Rhs>
        </Row>
        <Row popup="false" name="IB2">
          <Rhs vislevel="1" flags="1">
            <Toggle caption="Toggle between intial image and threshold result in IB3" value="true"/>
            <Subcolumn vislevel="0">
              <Row name="caption">
                <Rhs vislevel="0" flags="4">
                  <iText formula="&quot;Toggle between intial image and threshold result in IB3&quot;"/>
                </Rhs>
              </Row>
              <Row name="value">
                <Rhs vislevel="0" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="super">
                <Rhs vislevel="1" flags="4">
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Widget_toggle_item.action"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="0" y="862" open="true" selected="false" sform="false" next="5" name="JB" caption="(BIG) NUMBER of directions to ignore in the &quot;darker in all but n directions&quot; test (useable range: 0-7)">
      <Subcolumn vislevel="3">
        <Row popup="false" name="JB4">
          <Rhs vislevel="1" flags="4">
            <iText formula="&quot;There are 8 ranked directions because we symmetrized the octants themselves&quot;"/>
          </Rhs>
        </Row>
        <Row popup="false" name="JB1">
          <Rhs vislevel="1" flags="4">
            <iText formula="2"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="2137" y="0" open="true" selected="false" sform="false" next="7" name="AAA" caption="exclude small centroids too close to big centroids">
      <Subcolumn vislevel="3">
        <Row popup="false" name="AAA6">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="HB1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="AAA1">
          <Rhs vislevel="2" flags="5">
            <iImage window_x="0" window_y="0" window_width="1680" window_height="1050" image_left="812" image_top="485" image_mag="1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="true" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Filter_morphology_item.Dilate_item.Dilate4_item.action HB1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="AAA2">
          <Rhs vislevel="2" flags="5">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Filter_morphology_item.Dilate_item.Dilate8_item.action AAA1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="AAA3">
          <Rhs vislevel="2" flags="5">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Filter_morphology_item.Dilate_item.Dilate4_item.action AAA2"/>
          </Rhs>
        </Row>
        <Row popup="false" name="AAA4">
          <Rhs vislevel="2" flags="5">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Filter_morphology_item.Dilate_item.Dilate8_item.action AAA3"/>
          </Rhs>
        </Row>
        <Row popup="false" name="AAA5">
          <Rhs vislevel="2" flags="5">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Filter_morphology_item.Dilate_item.Dilate4_item.action AAA4"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="3041" y="0" open="true" selected="false" sform="false" next="22" name="Q" caption="(SMALL) disk mask">
      <Subcolumn vislevel="3">
        <Row popup="false" name="Q2">
          <Rhs vislevel="1" flags="4">
            <iText formula="LB1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="Q4">
          <Rhs vislevel="1" flags="4">
            <iText formula="2*Q2-1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="Q20">
          <Rhs vislevel="2" flags="5">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="J6"/>
          </Rhs>
        </Row>
        <Row popup="false" name="Q5">
          <Rhs vislevel="3" flags="7">
            <iImage window_x="0" window_y="25" window_width="117" window_height="136" image_left="42" image_top="39" image_mag="1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="true" scale="1" offset="0" falsecolour="false" type="true"/>
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
                  <Subcolumn vislevel="0">
                    <Row name="caption">
                      <Rhs vislevel="0" flags="4">
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="expr">
                      <Rhs vislevel="0" flags="4">
                        <iText formula="Q4"/>
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
                  <Expression caption="Image height (pixels)"/>
                  <Subcolumn vislevel="0">
                    <Row name="caption">
                      <Rhs vislevel="0" flags="4">
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="expr">
                      <Rhs vislevel="0" flags="4">
                        <iText formula="Q4"/>
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
            <iText formula="Pattern_images_item.Xy_item.action"/>
          </Rhs>
        </Row>
        <Row popup="false" name="Q12">
          <Rhs vislevel="2" flags="5">
            <iImage window_x="0" window_y="0" window_width="1674" window_height="999" image_left="205" image_top="117" image_mag="4" show_status="false" show_paintbox="false" show_convert="false" show_rulers="true" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="(Q5?0+1-Q2)**2+(Q5?1+1-Q2)**2 &lt; Q2**2"/>
          </Rhs>
        </Row>
        <Row popup="false" name="Q14">
          <Rhs vislevel="1" flags="1">
            <Matrix/>
            <Subcolumn vislevel="0"/>
            <iText formula="Matrix_to_matrix_item.action Q12"/>
          </Rhs>
        </Row>
        <Row popup="false" name="Q15">
          <Rhs vislevel="1" flags="4">
            <iText formula="sum Q14"/>
          </Rhs>
        </Row>
        <Row popup="false" name="Q16">
          <Rhs vislevel="0" flags="4">
            <Matrix/>
            <Subcolumn vislevel="0"/>
            <iText formula="(int) ((65535/Q15)*Q14)"/>
          </Rhs>
        </Row>
        <Row popup="false" name="Q17">
          <Rhs vislevel="1" flags="4">
            <iText formula="sum Q16"/>
          </Rhs>
        </Row>
        <Row popup="false" name="Q18">
          <Rhs vislevel="0" flags="4">
            <Matrix/>
            <Subcolumn vislevel="0"/>
            <iText formula="Matrix_con Q17 0 Q16.value"/>
          </Rhs>
        </Row>
        <Row popup="false" name="Q19">
          <Rhs vislevel="1" flags="4">
            <iText formula="take 1 (iterate rot45 Q18)"/>
          </Rhs>
        </Row>
        <Row popup="false" name="Q21">
          <Rhs vislevel="2" flags="5">
            <iText formula="conv Q19?0 Q20"/>
            <iImage window_x="0" window_y="0" window_width="750" window_height="750" image_left="347" image_top="335" image_mag="1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="true" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="0" y="2671" open="true" selected="false" sform="false" next="2" name="LB" caption="(SMALL) RADIUS of the inner disk">
      <Subcolumn vislevel="3">
        <Row popup="false" name="LB1">
          <Rhs vislevel="1" flags="4">
            <iText formula="3"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="0" y="2770" open="true" selected="false" sform="false" next="2" name="MB" caption="(SMALL) inner RADIUS of the outer ring">
      <Subcolumn vislevel="3">
        <Row popup="false" name="MB1">
          <Rhs vislevel="1" flags="4">
            <iText formula="6"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="0" y="2869" open="true" selected="false" sform="false" next="3" name="OB" caption="(SMALL) outer RADIUS of the outer ring">
      <Subcolumn vislevel="3">
        <Row popup="false" name="OB1">
          <Rhs vislevel="1" flags="4">
            <iText formula="8"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="4071" y="0" open="true" selected="false" sform="false" next="15" name="DB" caption="(SMALL) outer radius of ring">
      <Subcolumn vislevel="3">
        <Row popup="false" name="DB1">
          <Rhs vislevel="1" flags="4">
            <iText formula="OB1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="DB2">
          <Rhs vislevel="1" flags="4">
            <iText formula="2*DB1-1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="DB3">
          <Rhs vislevel="2" flags="5">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="J6"/>
          </Rhs>
        </Row>
        <Row popup="false" name="DB4">
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
                  <Subcolumn vislevel="0">
                    <Row name="caption">
                      <Rhs vislevel="0" flags="4">
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="expr">
                      <Rhs vislevel="0" flags="4">
                        <iText formula="DB2"/>
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
                  <Expression caption="Image height (pixels)"/>
                  <Subcolumn vislevel="0">
                    <Row name="caption">
                      <Rhs vislevel="0" flags="4">
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="expr">
                      <Rhs vislevel="0" flags="4">
                        <iText formula="DB2"/>
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
            <iText formula="Pattern_images_item.Xy_item.action"/>
          </Rhs>
        </Row>
        <Row popup="false" name="DB7">
          <Rhs vislevel="2" flags="5">
            <iImage window_x="0" window_y="0" window_width="1674" window_height="999" image_left="205" image_top="117" image_mag="4" show_status="false" show_paintbox="false" show_convert="false" show_rulers="true" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="(DB4?0+1-DB1)**2+(DB4?1+1-DB1)**2 &lt; DB1**2"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="4496" y="0" open="true" selected="false" sform="false" next="27" name="KB" caption="(SMALL) inner radius of ring">
      <Subcolumn vislevel="3">
        <Row popup="false" name="KB1">
          <Rhs vislevel="1" flags="4">
            <iText formula="MB1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="KB2">
          <Rhs vislevel="1" flags="4">
            <iText formula="2*KB1-1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="KB21">
          <Rhs vislevel="1" flags="4">
            <iText formula="DB1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="KB20">
          <Rhs vislevel="1" flags="4">
            <iText formula="DB2"/>
          </Rhs>
        </Row>
        <Row popup="false" name="KB3">
          <Rhs vislevel="2" flags="5">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="J6"/>
          </Rhs>
        </Row>
        <Row popup="false" name="KB4">
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
                  <Subcolumn vislevel="0">
                    <Row name="caption">
                      <Rhs vislevel="0" flags="4">
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="expr">
                      <Rhs vislevel="0" flags="4">
                        <iText formula="KB20"/>
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
                  <Expression caption="Image height (pixels)"/>
                  <Subcolumn vislevel="0">
                    <Row name="caption">
                      <Rhs vislevel="0" flags="4">
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="expr">
                      <Rhs vislevel="0" flags="4">
                        <iText formula="KB20"/>
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
            <iText formula="Pattern_images_item.Xy_item.action"/>
          </Rhs>
        </Row>
        <Row popup="false" name="KB7">
          <Rhs vislevel="2" flags="5">
            <iImage window_x="0" window_y="0" window_width="1674" window_height="999" image_left="205" image_top="117" image_mag="4" show_status="false" show_paintbox="false" show_convert="false" show_rulers="true" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="(KB4?0+1-KB21)**2+(KB4?1+1-KB21)**2 &lt; KB1**2"/>
          </Rhs>
        </Row>
        <Row popup="false" name="KB23">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="DB7 &amp; ~KB7"/>
          </Rhs>
        </Row>
        <Row popup="false" name="KB24">
          <Rhs vislevel="2" flags="5">
            <iImage window_x="0" window_y="25" window_width="364" window_height="204" image_left="166" image_top="39" image_mag="1" show_status="true" show_paintbox="false" show_convert="true" show_rulers="true" scale="128" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="D6"/>
          </Rhs>
        </Row>
        <Row popup="false" name="KB25">
          <Rhs vislevel="2" flags="5">
            <iImage window_x="0" window_y="25" window_width="388" window_height="204" image_left="178" image_top="39" image_mag="1" show_status="true" show_paintbox="false" show_convert="true" show_rulers="true" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="KB24*KB23"/>
          </Rhs>
        </Row>
        <Row popup="false" name="KB26">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Image_transform_item.Flip_item.Left_right_item.action KB25"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="8534" y="0" open="true" selected="false" sform="false" next="13" name="K" caption="(BIG) disk mask">
      <Subcolumn vislevel="3">
        <Row popup="false" name="K1">
          <Rhs vislevel="1" flags="4">
            <iText formula="PB1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="K2">
          <Rhs vislevel="1" flags="4">
            <iText formula="2*K1-1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="K3">
          <Rhs vislevel="2" flags="5">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="J6"/>
          </Rhs>
        </Row>
        <Row popup="false" name="K4">
          <Rhs vislevel="3" flags="7">
            <iImage window_x="0" window_y="25" window_width="117" window_height="136" image_left="42" image_top="39" image_mag="1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="true" scale="1" offset="0" falsecolour="false" type="true"/>
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
                  <Subcolumn vislevel="0">
                    <Row name="caption">
                      <Rhs vislevel="0" flags="4">
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="expr">
                      <Rhs vislevel="0" flags="4">
                        <iText formula="K2"/>
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
                  <Expression caption="Image height (pixels)"/>
                  <Subcolumn vislevel="0">
                    <Row name="caption">
                      <Rhs vislevel="0" flags="4">
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="expr">
                      <Rhs vislevel="0" flags="4">
                        <iText formula="K2"/>
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
            <iText formula="Pattern_images_item.Xy_item.action"/>
          </Rhs>
        </Row>
        <Row popup="false" name="K5">
          <Rhs vislevel="2" flags="5">
            <iImage window_x="0" window_y="0" window_width="1674" window_height="999" image_left="205" image_top="117" image_mag="4" show_status="false" show_paintbox="false" show_convert="false" show_rulers="true" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="(K4?0+1-K1)**2+(K4?1+1-K1)**2 &lt; K1**2"/>
          </Rhs>
        </Row>
        <Row popup="false" name="K6">
          <Rhs vislevel="0" flags="4">
            <Matrix/>
            <Subcolumn vislevel="0"/>
            <iText formula="Matrix_to_matrix_item.action K5"/>
          </Rhs>
        </Row>
        <Row popup="false" name="K7">
          <Rhs vislevel="1" flags="4">
            <iText formula="sum K6"/>
          </Rhs>
        </Row>
        <Row popup="false" name="K8">
          <Rhs vislevel="0" flags="4">
            <Matrix/>
            <Subcolumn vislevel="0"/>
            <iText formula="(int) ((65535/K7)*K6)"/>
          </Rhs>
        </Row>
        <Row popup="false" name="K9">
          <Rhs vislevel="1" flags="4">
            <iText formula="sum K8"/>
          </Rhs>
        </Row>
        <Row popup="false" name="K10">
          <Rhs vislevel="0" flags="4">
            <Matrix/>
            <Subcolumn vislevel="0"/>
            <iText formula="Matrix_con K9 0 K8.value"/>
          </Rhs>
        </Row>
        <Row popup="false" name="K11">
          <Rhs vislevel="1" flags="4">
            <iText formula="take 1 (iterate rot45 K10)"/>
          </Rhs>
        </Row>
        <Row popup="false" name="K12">
          <Rhs vislevel="2" flags="5">
            <iText formula="conv K11?0 K3"/>
            <iImage window_x="0" window_y="25" window_width="750" window_height="722" image_left="2022" image_top="1367" image_mag="1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="true" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="0" y="3067" open="true" selected="false" sform="false" next="2" name="NB" caption="(BIG) inner RADIUS of the outer ring">
      <Subcolumn vislevel="3">
        <Row popup="false" name="NB1">
          <Rhs vislevel="1" flags="4">
            <iText formula="12"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="0" y="2968" open="true" selected="false" sform="false" next="2" name="PB" caption="(BIG) RADIUS of the inner disk">
      <Subcolumn vislevel="3">
        <Row popup="false" name="PB1">
          <Rhs vislevel="1" flags="4">
            <iText formula="12"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="0" y="3166" open="true" selected="false" sform="false" next="2" name="QB" caption="(BIG) outer RADIUS of the outer ring">
      <Subcolumn vislevel="3">
        <Row popup="false" name="QB1">
          <Rhs vislevel="1" flags="4">
            <iText formula="15"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="9528" y="0" open="true" selected="false" sform="false" next="6" name="RB" caption="(BIG) outer radius of ring">
      <Subcolumn vislevel="3">
        <Row popup="false" name="RB1">
          <Rhs vislevel="1" flags="4">
            <iText formula="QB1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="RB2">
          <Rhs vislevel="1" flags="4">
            <iText formula="2*RB1-1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="RB3">
          <Rhs vislevel="2" flags="5">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="J6"/>
          </Rhs>
        </Row>
        <Row popup="false" name="RB4">
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
                  <Subcolumn vislevel="0">
                    <Row name="caption">
                      <Rhs vislevel="0" flags="4">
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="expr">
                      <Rhs vislevel="0" flags="4">
                        <iText formula="RB2"/>
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
                  <Expression caption="Image height (pixels)"/>
                  <Subcolumn vislevel="0">
                    <Row name="caption">
                      <Rhs vislevel="0" flags="4">
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="expr">
                      <Rhs vislevel="0" flags="4">
                        <iText formula="RB2"/>
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
            <iText formula="Pattern_images_item.Xy_item.action"/>
          </Rhs>
        </Row>
        <Row popup="false" name="RB5">
          <Rhs vislevel="2" flags="5">
            <iImage window_x="0" window_y="0" window_width="1674" window_height="999" image_left="205" image_top="117" image_mag="4" show_status="false" show_paintbox="false" show_convert="false" show_rulers="true" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="(RB4?0+1-RB1)**2+(RB4?1+1-RB1)**2 &lt; RB1**2"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="9952" y="0" open="true" selected="false" sform="false" next="13" name="SB" caption="(BIG) inner radius of ring">
      <Subcolumn vislevel="3">
        <Row popup="false" name="SB1">
          <Rhs vislevel="1" flags="4">
            <iText formula="NB1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="SB2">
          <Rhs vislevel="1" flags="4">
            <iText formula="2*SB1-1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="SB3">
          <Rhs vislevel="1" flags="4">
            <iText formula="RB1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="SB4">
          <Rhs vislevel="1" flags="4">
            <iText formula="RB2"/>
          </Rhs>
        </Row>
        <Row popup="false" name="SB5">
          <Rhs vislevel="2" flags="5">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="J6"/>
          </Rhs>
        </Row>
        <Row popup="false" name="SB6">
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
                  <Subcolumn vislevel="0">
                    <Row name="caption">
                      <Rhs vislevel="0" flags="4">
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="expr">
                      <Rhs vislevel="0" flags="4">
                        <iText formula="SB4"/>
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
                  <Expression caption="Image height (pixels)"/>
                  <Subcolumn vislevel="0">
                    <Row name="caption">
                      <Rhs vislevel="0" flags="4">
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="expr">
                      <Rhs vislevel="0" flags="4">
                        <iText formula="SB4"/>
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
            <iText formula="Pattern_images_item.Xy_item.action"/>
          </Rhs>
        </Row>
        <Row popup="false" name="SB7">
          <Rhs vislevel="2" flags="5">
            <iImage window_x="0" window_y="0" window_width="1674" window_height="999" image_left="205" image_top="117" image_mag="4" show_status="false" show_paintbox="false" show_convert="false" show_rulers="true" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="(SB6?0+1-SB3)**2+(SB6?1+1-SB3)**2 &lt; SB1**2"/>
          </Rhs>
        </Row>
        <Row popup="false" name="SB8">
          <Rhs vislevel="2" flags="5">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="RB5 &amp; ~SB7"/>
          </Rhs>
        </Row>
        <Row popup="false" name="SB9">
          <Rhs vislevel="2" flags="5">
            <iImage window_x="0" window_y="25" window_width="364" window_height="208" image_left="165" image_top="39" image_mag="1" show_status="true" show_paintbox="false" show_convert="true" show_rulers="true" scale="128" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Z10"/>
          </Rhs>
        </Row>
        <Row popup="false" name="SB10">
          <Rhs vislevel="2" flags="5">
            <iImage window_x="0" window_y="0" window_width="1278" window_height="970" image_left="9" image_top="6" image_mag="64" show_status="true" show_paintbox="false" show_convert="true" show_rulers="true" scale="0.39938338503447723" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="SB9*SB8"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="0" y="457" open="true" selected="false" sform="false" next="1" name="ZZZZ" caption="THE MAIN ADJUSTABLE PARAMETERS ARE FOUND BELOW">
      <Subcolumn vislevel="3"/>
    </Column>
    <Column x="774" y="0" open="true" selected="false" sform="false" next="1" name="YYYY" caption="THE MAIN RESULTS ARE FOUND BELOW">
      <Subcolumn vislevel="3"/>
    </Column>
    <Column x="1621" y="0" open="true" selected="false" sform="false" next="1" name="XXXX" caption="THE MAIN COMPUTATION STEPS ARE FOUND TO THE RIGHT">
      <Subcolumn vislevel="3"/>
    </Column>
    <Column x="774" y="240" open="true" selected="false" sform="false" next="10" name="WWWW" caption="NANOPARTICLE COORDINATES">
      <Subcolumn vislevel="3">
        <Row popup="false" name="WWWW2">
          <Rhs vislevel="1" flags="4">
            <iText formula="&quot;coordinates of large blobs&quot;"/>
          </Rhs>
        </Row>
        <Row popup="false" name="WWWW1">
          <Rhs vislevel="1" flags="1">
            <Matrix/>
            <Subcolumn vislevel="0"/>
            <iText formula="AB11"/>
          </Rhs>
        </Row>
        <Row popup="false" name="WWWW4">
          <Rhs vislevel="1" flags="4">
            <iText formula="&quot;coordinates of small blobs&quot;"/>
          </Rhs>
        </Row>
        <Row popup="false" name="WWWW3">
          <Rhs vislevel="2" flags="5">
            <Matrix/>
            <Subcolumn vislevel="0"/>
            <iText formula="R10"/>
          </Rhs>
        </Row>
        <Row popup="false" name="WWWW5">
          <Rhs vislevel="1" flags="4">
            <iText formula="&quot; &quot;"/>
          </Rhs>
        </Row>
        <Row popup="false" name="WWWW6">
          <Rhs vislevel="1" flags="4">
            <iText formula="&quot;verify that the correct number of blobs were detected&quot;"/>
          </Rhs>
        </Row>
        <Row popup="false" name="WWWW7">
          <Rhs vislevel="1" flags="4">
            <iText formula="if WWWW1.width != 10 then error &quot;wrong number of large blobs detected&quot; else &quot;large ok!&quot;"/>
          </Rhs>
        </Row>
        <Row popup="false" name="WWWW9">
          <Rhs vislevel="1" flags="4">
            <iText formula="if WWWW3.width != 3 then error &quot;wrong number of small blobs detected&quot; else &quot;small ok!&quot;"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="0" y="2605" open="true" selected="false" sform="false" next="1" name="VVVV" caption="THE FOLLOWING PARAMETERS ARE GEOMETRICAL IN NATURE. HANDLE WITH CARE.">
      <Subcolumn vislevel="3"/>
    </Column>
  </Workspace>
</root>



