<?xml version="1.0"?>
<root xmlns="http://www.vips.ecs.soton.ac.uk/nip/7.23.0">
  <Workspace window_x="0" window_y="0" window_width="788" window_height="769" filename="test_morph.ws" view="WORKSPACE_MODE_REGULAR" scale="1" offset="0" lpane_position="100" lpane_open="false" rpane_position="400" rpane_open="false" local_defs="// private definitions for this workspace&#10;" name="test_morph" caption="Default empty workspace">
    <Column x="0" y="0" open="true" selected="false" sform="false" next="7" name="B" caption="test image">
      <Subcolumn vislevel="3">
        <Row popup="false" name="B1">
          <Rhs vislevel="1" flags="1">
            <iImage window_x="449" window_y="50" window_width="547" window_height="727" image_left="141" image_top="107" image_mag="8" show_status="true" show_paintbox="false" show_convert="false" show_rulers="false" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Image_file &quot;$VIPSHOME/share/nip2/data/examples/businesscard/slanted_oval_vase2.jpg&quot;"/>
          </Rhs>
        </Row>
        <Row popup="false" name="B3">
          <Rhs vislevel="2" flags="5">
            <iImage window_x="577" window_y="1" window_width="510" window_height="727" image_left="266" image_top="326" image_mag="1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="true" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="B1?1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="B4">
          <Rhs vislevel="1" flags="1">
            <iRegion image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true">
              <iRegiongroup/>
            </iRegion>
            <Subcolumn vislevel="0"/>
            <iText formula="Region B3 95 71 73 69"/>
          </Rhs>
        </Row>
        <Row popup="false" name="B5">
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
              <Row name="window_width">
                <Rhs vislevel="1" flags="1">
                  <Expression caption="Window width"/>
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
              <Row name="window_height">
                <Rhs vislevel="1" flags="1">
                  <Expression caption="Window height"/>
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
            </Subcolumn>
            <iText formula="Filter_enhance_item.Hist_equal_item.Local_item.action B3"/>
          </Rhs>
        </Row>
        <Row popup="false" name="B6">
          <Rhs vislevel="1" flags="1">
            <iImage window_x="577" window_y="1" window_width="510" window_height="727" image_left="230" image_top="326" image_mag="1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="true" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="B5&gt;128"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="834" y="0" open="true" selected="true" sform="false" next="7" name="C" caption="convolve with vips">
      <Subcolumn vislevel="3">
        <Row popup="false" name="C1">
          <Rhs vislevel="2" flags="4">
            <iText formula="map (converse erode B6) G8"/>
          </Rhs>
        </Row>
        <Row popup="false" name="C6">
          <Rhs vislevel="1" flags="1">
            <iImage window_x="768" window_y="1" window_width="510" window_height="727" image_left="230" image_top="326" image_mag="1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="true" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="foldr1 bitwise_or C1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="C2">
          <Rhs vislevel="2" flags="5">
            <iImage window_x="0" window_y="1" window_width="510" window_height="727" image_left="240" image_top="318" image_mag="1" show_status="true" show_paintbox="false" show_convert="false" show_rulers="false" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="conv A4 B1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="C3">
          <Rhs vislevel="2" flags="5">
            <iImage window_x="0" window_y="1" window_width="510" window_height="727" image_left="240" image_top="318" image_mag="1" show_status="true" show_paintbox="false" show_convert="false" show_rulers="false" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="conv F4 B1"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="1386" y="590" open="true" selected="false" sform="false" next="20" name="D" caption="convolve by hand">
      <Subcolumn vislevel="3">
        <Row popup="false" name="D19">
          <Rhs vislevel="0" flags="4">
            <Matrix/>
            <Subcolumn vislevel="0"/>
            <iText formula="A4"/>
          </Rhs>
        </Row>
        <Row popup="false" name="D1">
          <Rhs vislevel="2" flags="5">
            <iRegion image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true">
              <iRegiongroup/>
            </iRegion>
            <Subcolumn vislevel="0"/>
            <iText formula="Region B1 137 102 D19.width D19.height"/>
          </Rhs>
        </Row>
        <Row popup="false" name="D2">
          <Rhs vislevel="2" flags="5">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="D1?1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="D3">
          <Rhs vislevel="2" flags="5">
            <Matrix/>
            <Subcolumn vislevel="0"/>
            <iText formula="Matrix_to_matrix_item.action D2"/>
          </Rhs>
        </Row>
        <Row popup="false" name="D4">
          <Rhs vislevel="2" flags="5">
            <iText formula="Matrix (map2 (map2 multiply) D19.value D3.value)"/>
            <Matrix/>
            <Subcolumn vislevel="0"/>
          </Rhs>
        </Row>
        <Row popup="false" name="D5">
          <Rhs vislevel="1" flags="4">
            <iText formula="sum D4"/>
          </Rhs>
        </Row>
        <Row popup="false" name="D6">
          <Rhs vislevel="1" flags="4">
            <iText formula="(int) ((D5 + D19.scale / 2) / D19.scale + D19.offset)"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="2097" y="0" open="true" selected="false" sform="false" next="20" name="E" caption="check result">
      <Subcolumn vislevel="3">
        <Row popup="false" name="E16">
          <Rhs vislevel="0" flags="4">
            <iRegion image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true">
              <iRegiongroup/>
            </iRegion>
            <Subcolumn vislevel="0"/>
            <iText formula="H2"/>
          </Rhs>
        </Row>
        <Row popup="false" name="E17">
          <Rhs vislevel="1" flags="4">
            <iText formula="H7"/>
          </Rhs>
        </Row>
        <Row popup="false" name="E19">
          <Rhs vislevel="0" flags="4">
            <iText formula="C1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="E18">
          <Rhs vislevel="1" flags="4">
            <iText formula="&quot; &quot;"/>
          </Rhs>
        </Row>
        <Row popup="false" name="E8">
          <Rhs vislevel="0" flags="4">
            <iRegion image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true">
              <iRegiongroup/>
            </iRegion>
            <Subcolumn vislevel="0"/>
            <iText formula="Region E19 E16.left E16.top E16.width E16.height"/>
          </Rhs>
        </Row>
        <Row popup="false" name="E9">
          <Rhs vislevel="0" flags="4">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="E8?1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="E10">
          <Rhs vislevel="2" flags="5">
            <Matrix/>
            <Subcolumn vislevel="0"/>
            <iText formula="Matrix_to_matrix_item.action E9"/>
          </Rhs>
        </Row>
        <Row popup="false" name="E11">
          <Rhs vislevel="1" flags="4">
            <iText formula="(int) (E16.width / 2)"/>
          </Rhs>
        </Row>
        <Row popup="false" name="E12">
          <Rhs vislevel="1" flags="4">
            <iText formula="(int) (E16.height / 2)"/>
          </Rhs>
        </Row>
        <Row popup="false" name="E13">
          <Rhs vislevel="1" flags="4">
            <iText formula="E10.value?E11?E12"/>
          </Rhs>
        </Row>
        <Row popup="false" name="E14">
          <Rhs vislevel="1" flags="4">
            <iText formula="&quot; &quot;"/>
          </Rhs>
        </Row>
        <Row popup="false" name="E15">
          <Rhs vislevel="1" flags="4">
            <iText formula="if E13 != E17 then error &quot;match failed&quot; else &quot;ok&quot;"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="0" y="1036" open="true" selected="false" sform="false" next="5" name="F" caption="large test matrix">
      <Subcolumn vislevel="3">
        <Row popup="false" name="F1">
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
              <Row name="s">
                <Rhs vislevel="1" flags="1">
                  <Slider caption="Sigma" from="0.001" to="10" value="2.7010267062314539"/>
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
              <Row name="integer">
                <Rhs vislevel="1" flags="1">
                  <Toggle caption="Integer" value="true"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Matrix_build_item.Matrix_gaussian_item.action"/>
          </Rhs>
        </Row>
        <Row popup="false" name="F2">
          <Rhs vislevel="2" flags="5">
            <Matrix/>
            <Subcolumn vislevel="0"/>
            <iText formula="(int) (F1/10)"/>
          </Rhs>
        </Row>
        <Row popup="false" name="F3">
          <Rhs vislevel="1" flags="4">
            <iText formula="sum F2"/>
          </Rhs>
        </Row>
        <Row popup="false" name="F4">
          <Rhs vislevel="2" flags="5">
            <Matrix/>
            <Subcolumn vislevel="0"/>
            <iText formula="Matrix_con F3 0 F2.value"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="0" y="584" open="true" selected="false" sform="false" next="10" name="G" caption="small test matrix">
      <Subcolumn vislevel="3">
        <Row popup="false" name="G7">
          <Rhs vislevel="1" flags="1">
            <Matrix valuen="9" value0="128" value1="0" value2="128" value3="0" value4="255" value5="0" value6="0" value7="255" value8="0" width="3" height="3" scale="1" offset="0" filename="" display="2"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Matrix_build_item.Morphology_item.action"/>
          </Rhs>
        </Row>
        <Row popup="false" name="G8">
          <Rhs vislevel="1" flags="4">
            <iText formula="take 8 (iterate rot45 G7)"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="1386" y="0" open="true" selected="false" sform="false" next="8" name="H" caption="convolve by hand">
      <Subcolumn vislevel="3">
        <Row popup="false" name="H1">
          <Rhs vislevel="0" flags="4">
            <Matrix/>
            <Subcolumn vislevel="0"/>
            <iText formula="G5"/>
          </Rhs>
        </Row>
        <Row popup="false" name="H2">
          <Rhs vislevel="2" flags="5">
            <iRegion image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true">
              <iRegiongroup/>
            </iRegion>
            <Subcolumn vislevel="0"/>
            <iText formula="Region B1 137 102 H1.width H1.height"/>
          </Rhs>
        </Row>
        <Row popup="false" name="H3">
          <Rhs vislevel="2" flags="5">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="H2?1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="H4">
          <Rhs vislevel="2" flags="5">
            <Matrix/>
            <Subcolumn vislevel="0"/>
            <iText formula="Matrix_to_matrix_item.action H3"/>
          </Rhs>
        </Row>
        <Row popup="false" name="H5">
          <Rhs vislevel="2" flags="5">
            <iText formula="Matrix (map2 (map2 multiply) H1.value H4.value)"/>
            <Matrix/>
            <Subcolumn vislevel="0"/>
          </Rhs>
        </Row>
        <Row popup="false" name="H6">
          <Rhs vislevel="1" flags="4">
            <iText formula="sum H5"/>
          </Rhs>
        </Row>
        <Row popup="false" name="H7">
          <Rhs vislevel="1" flags="4">
            <iText formula="(int) ((H6 + H1.scale / 2) / H1.scale + H1.offset)"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="1386" y="1276" open="true" selected="false" sform="false" next="8" name="I" caption="convolve by hand">
      <Subcolumn vislevel="3">
        <Row popup="false" name="I1">
          <Rhs vislevel="0" flags="4">
            <Matrix/>
            <Subcolumn vislevel="0"/>
            <iText formula="F4"/>
          </Rhs>
        </Row>
        <Row popup="false" name="I2">
          <Rhs vislevel="2" flags="5">
            <iRegion image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true">
              <iRegiongroup/>
            </iRegion>
            <Subcolumn vislevel="0"/>
            <iText formula="Region B1 137 102 I1.width I1.height"/>
          </Rhs>
        </Row>
        <Row popup="false" name="I3">
          <Rhs vislevel="2" flags="5">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="I2?1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="I4">
          <Rhs vislevel="2" flags="5">
            <Matrix/>
            <Subcolumn vislevel="0"/>
            <iText formula="Matrix_to_matrix_item.action I3"/>
          </Rhs>
        </Row>
        <Row popup="false" name="I5">
          <Rhs vislevel="2" flags="5">
            <iText formula="Matrix (map2 (map2 multiply) I1.value I4.value)"/>
            <Matrix/>
            <Subcolumn vislevel="0"/>
          </Rhs>
        </Row>
        <Row popup="false" name="I6">
          <Rhs vislevel="1" flags="4">
            <iText formula="sum I5"/>
          </Rhs>
        </Row>
        <Row popup="false" name="I7">
          <Rhs vislevel="1" flags="4">
            <iText formula="(int) ((I6 + I1.scale / 2) / I1.scale + I1.offset)"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="2097" y="522" open="true" selected="false" sform="false" next="13" name="N" caption="check result">
      <Subcolumn vislevel="3">
        <Row popup="false" name="N1">
          <Rhs vislevel="0" flags="4">
            <iRegion image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true">
              <iRegiongroup/>
            </iRegion>
            <Subcolumn vislevel="0"/>
            <iText formula="D1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="N2">
          <Rhs vislevel="1" flags="4">
            <iText formula="D6"/>
          </Rhs>
        </Row>
        <Row popup="false" name="N3">
          <Rhs vislevel="0" flags="4">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="C2"/>
          </Rhs>
        </Row>
        <Row popup="false" name="N4">
          <Rhs vislevel="1" flags="4">
            <iText formula="&quot; &quot;"/>
          </Rhs>
        </Row>
        <Row popup="false" name="N5">
          <Rhs vislevel="0" flags="4">
            <iRegion image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true">
              <iRegiongroup/>
            </iRegion>
            <Subcolumn vislevel="0"/>
            <iText formula="Region N3 N1.left N1.top N1.width N1.height"/>
          </Rhs>
        </Row>
        <Row popup="false" name="N6">
          <Rhs vislevel="0" flags="4">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="N5?1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="N7">
          <Rhs vislevel="2" flags="5">
            <Matrix/>
            <Subcolumn vislevel="0"/>
            <iText formula="Matrix_to_matrix_item.action N6"/>
          </Rhs>
        </Row>
        <Row popup="false" name="N8">
          <Rhs vislevel="1" flags="4">
            <iText formula="(int) (N1.width / 2)"/>
          </Rhs>
        </Row>
        <Row popup="false" name="N9">
          <Rhs vislevel="1" flags="4">
            <iText formula="(int) (N1.height / 2)"/>
          </Rhs>
        </Row>
        <Row popup="false" name="N10">
          <Rhs vislevel="1" flags="4">
            <iText formula="N7.value?N8?N9"/>
          </Rhs>
        </Row>
        <Row popup="false" name="N11">
          <Rhs vislevel="1" flags="4">
            <iText formula="&quot; &quot;"/>
          </Rhs>
        </Row>
        <Row popup="false" name="N12">
          <Rhs vislevel="1" flags="4">
            <iText formula="if N10 != N2 then error &quot;match failed&quot; else &quot;ok&quot;"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="2097" y="1092" open="true" selected="false" sform="false" next="13" name="O" caption="check result">
      <Subcolumn vislevel="3">
        <Row popup="false" name="O1">
          <Rhs vislevel="0" flags="4">
            <iRegion image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true">
              <iRegiongroup/>
            </iRegion>
            <Subcolumn vislevel="0"/>
            <iText formula="I2"/>
          </Rhs>
        </Row>
        <Row popup="false" name="O2">
          <Rhs vislevel="1" flags="4">
            <iText formula="I7"/>
          </Rhs>
        </Row>
        <Row popup="false" name="O3">
          <Rhs vislevel="0" flags="4">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="C3"/>
          </Rhs>
        </Row>
        <Row popup="false" name="O4">
          <Rhs vislevel="1" flags="4">
            <iText formula="&quot; &quot;"/>
          </Rhs>
        </Row>
        <Row popup="false" name="O5">
          <Rhs vislevel="0" flags="4">
            <iRegion image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true">
              <iRegiongroup/>
            </iRegion>
            <Subcolumn vislevel="0"/>
            <iText formula="Region O3 O1.left O1.top O1.width O1.height"/>
          </Rhs>
        </Row>
        <Row popup="false" name="O6">
          <Rhs vislevel="0" flags="4">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="O5?1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="O7">
          <Rhs vislevel="2" flags="5">
            <Matrix/>
            <Subcolumn vislevel="0"/>
            <iText formula="Matrix_to_matrix_item.action O6"/>
          </Rhs>
        </Row>
        <Row popup="false" name="O8">
          <Rhs vislevel="1" flags="4">
            <iText formula="(int) (O1.width / 2)"/>
          </Rhs>
        </Row>
        <Row popup="false" name="O9">
          <Rhs vislevel="1" flags="4">
            <iText formula="(int) (O1.height / 2)"/>
          </Rhs>
        </Row>
        <Row popup="false" name="O10">
          <Rhs vislevel="1" flags="4">
            <iText formula="O7.value?O8?O9"/>
          </Rhs>
        </Row>
        <Row popup="false" name="O11">
          <Rhs vislevel="1" flags="4">
            <iText formula="&quot; &quot;"/>
          </Rhs>
        </Row>
        <Row popup="false" name="O12">
          <Rhs vislevel="1" flags="4">
            <iText formula="if O10 != O2 then error &quot;match failed&quot; else &quot;ok&quot;"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="0" y="778" open="true" selected="false" sform="false" next="3" name="A" caption="medium test matrix">
      <Subcolumn vislevel="3">
        <Row popup="false" name="A1">
          <Rhs vislevel="0" flags="4">
            <Matrix/>
            <Subcolumn vislevel="0"/>
            <iText formula="G7"/>
          </Rhs>
        </Row>
        <Row popup="false" name="A2">
          <Rhs vislevel="1" flags="1">
            <Matrix/>
            <Subcolumn vislevel="0"/>
            <iText formula="dilate G7 G7"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
  </Workspace>
</root>


