<?xml version="1.0"?>
<root xmlns="http://www.vips.ecs.soton.ac.uk/nip/8.5.0">
  <Workspace filename="test_matrix.ws" view="WORKSPACE_MODE_REGULAR" scale="1" offset="0" window_width="1920" window_height="1165" lpane_position="400" lpane_open="false" rpane_position="100" rpane_open="false" local_defs="// private definitions for this workspace&#10;" name="test_matrix" caption="Default empty workspace">
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
    <Column x="476" y="0" open="true" selected="false" sform="false" next="7" name="TB" caption="Matrix / New">
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
    <Column x="1105" y="0" open="true" selected="false" sform="false" next="5" name="VB" caption="Matrix / Extract">
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
    <Column x="1486" y="0" open="true" selected="false" sform="false" next="4" name="WB" caption="Matrix / Insert">
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
    <Column x="1879" y="0" open="true" selected="false" sform="false" next="4" name="XB" caption="Matrix / Delete">
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
    <Column x="2795" y="0" open="true" selected="false" sform="false" next="6" name="YB" caption="Matrix / Rotate">
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
    <Column x="3206" y="0" open="true" selected="false" sform="false" next="4" name="ZB" caption="Matrix / Flip">
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
    <Column x="3518" y="0" open="true" selected="true" sform="false" next="14" name="AC" caption="Matrix">
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
        <Row popup="false" name="AC13">
          <Rhs vislevel="1" flags="1">
            <iImage window_x="561" window_y="29" window_width="472" window_height="390" image_left="220" image_top="139" image_mag="1" show_status="true" show_paintbox="false" show_convert="true" show_rulers="false" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Matrix_buildlut_item.action AC10"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="2253" y="0" open="true" selected="false" sform="false" next="6" name="A" caption="Matrix / Join">
      <Subcolumn vislevel="3">
        <Row popup="false" name="A1">
          <Rhs vislevel="0" flags="4">
            <Matrix/>
            <Subcolumn vislevel="0"/>
            <iText formula="TB1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="A2">
          <Rhs vislevel="0" flags="4">
            <Matrix/>
            <Subcolumn vislevel="0"/>
            <iText formula="TB2"/>
          </Rhs>
        </Row>
        <Row popup="false" name="A3">
          <Rhs vislevel="2" flags="5">
            <Matrix/>
            <Subcolumn vislevel="0"/>
            <iText formula="Matrix_join.Left_right_item.action A1 A2"/>
          </Rhs>
        </Row>
        <Row popup="false" name="A4">
          <Rhs vislevel="2" flags="5">
            <Matrix/>
            <Subcolumn vislevel="0"/>
            <iText formula="Matrix_join.Top_bottom_item.action A1 A2"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
  </Workspace>
</root>



