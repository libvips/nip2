<?xml version="1.0"?>
<root xmlns="http://www.vips.ecs.soton.ac.uk/nip/7.28.0">
  <Workspace window_x="0" window_y="0" window_width="928" window_height="743" filename="/home/john/Desktop/test.ws" view="WORKSPACE_MODE_REGULAR" scale="1" offset="0" lpane_position="100" lpane_open="false" rpane_position="400" rpane_open="false" local_defs="// private definitions for this workspace&#10;" name="test" caption="Default empty workspace">
    <Column x="0" y="0" open="true" selected="true" sform="false" next="28" name="A">
      <Subcolumn vislevel="3">
        <Row popup="false" name="A1">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Image_file &quot;$VIPSHOME/share/nip2/data/examples/businesscard/slanted_oval_vase2.jpg&quot;"/>
          </Rhs>
        </Row>
        <Row popup="false" name="A3">
          <Rhs vislevel="2" flags="5">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="A1 * 8.1 - 100"/>
          </Rhs>
        </Row>
        <Row popup="false" name="A8">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="A3 ++ A3 * 0.6"/>
          </Rhs>
        </Row>
        <Row popup="false" name="A10">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="A8 ++ (0.1 * A8)"/>
          </Rhs>
        </Row>
        <Row popup="false" name="A11">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="A10 ++ (A10 * 1.3)"/>
          </Rhs>
        </Row>
        <Row popup="false" name="A12">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="A11 ++ (A11 * 0.7)"/>
          </Rhs>
        </Row>
        <Row popup="false" name="A4">
          <Rhs vislevel="2" flags="5">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Image_number_format_item.S32_item.action A12"/>
          </Rhs>
        </Row>
        <Row popup="false" name="A13">
          <Rhs vislevel="1" flags="1">
            <iRegion image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true" left="508" top="148" width="496" height="476">
              <iRegiongroup/>
            </iRegion>
            <Subcolumn vislevel="0"/>
            <iText formula="Region A4 4018 3668 2184 1694"/>
          </Rhs>
        </Row>
        <Row popup="false" name="A14">
          <Rhs vislevel="1" flags="1">
            <Matrix/>
            <Subcolumn vislevel="0"/>
            <iText formula="Math_stats_item.Stats_item.action A4"/>
          </Rhs>
        </Row>
        <Row popup="false" name="A15">
          <Rhs vislevel="1" flags="1">
            <Matrix/>
            <Subcolumn vislevel="0"/>
            <iText formula="Math_stats_item.Stats_item.action A13"/>
          </Rhs>
        </Row>
        <Row popup="false" name="A17">
          <Rhs vislevel="3" flags="7">
            <iText formula="Vector (transpose (A14.extract 0 0 1 49).value)?0"/>
            <Subcolumn vislevel="1"/>
            <Vector/>
          </Rhs>
        </Row>
        <Row popup="false" name="A18">
          <Rhs vislevel="3" flags="7">
            <iText formula="Vector (transpose (A15.extract 0 0 1 49).value)?0"/>
            <Subcolumn vislevel="1"/>
            <Vector/>
          </Rhs>
        </Row>
        <Row popup="false" name="A19">
          <Rhs vislevel="2" flags="5">
            <Subcolumn vislevel="0"/>
            <iText formula="A18 &gt;= A17"/>
            <Vector/>
          </Rhs>
        </Row>
        <Row popup="false" name="A20">
          <Rhs vislevel="1" flags="4">
            <iText formula="min A19"/>
          </Rhs>
        </Row>
        <Row popup="false" name="A22">
          <Rhs vislevel="3" flags="7">
            <iText formula="Vector (transpose (A14.extract 1 0 1 49).value)?0"/>
            <Subcolumn vislevel="1"/>
            <Vector/>
          </Rhs>
        </Row>
        <Row popup="false" name="A23">
          <Rhs vislevel="3" flags="7">
            <iText formula="Vector (transpose (A15.extract 1 0 1 49).value)?0"/>
            <Subcolumn vislevel="1"/>
            <Vector/>
          </Rhs>
        </Row>
        <Row popup="false" name="A24">
          <Rhs vislevel="2" flags="5">
            <Subcolumn vislevel="0"/>
            <iText formula="A23 &lt;= A23"/>
            <Vector/>
          </Rhs>
        </Row>
        <Row popup="false" name="A25">
          <Rhs vislevel="1" flags="4">
            <iText formula="min A24"/>
          </Rhs>
        </Row>
        <Row popup="false" name="A27">
          <Rhs vislevel="1" flags="4">
            <iText formula="if A20 != 255 || A25 != 255 then error &quot;VipsStats failed!&quot; else &quot;ok&quot;"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
  </Workspace>
</root>



