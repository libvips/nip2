<?xml version="1.0"?>
<root xmlns="http://www.vips.ecs.soton.ac.uk/nip/8.5.0">
  <Workspace window_x="0" window_y="0" window_width="928" window_height="743" filename="test_stats.ws" view="WORKSPACE_MODE_REGULAR" scale="1" offset="0" lpane_position="100" lpane_open="false" rpane_position="400" rpane_open="false" local_defs="// private definitions for this workspace&#10;" name="test_stats" caption="Default empty workspace">
    <Column x="0" y="0" open="true" selected="false" sform="false" next="30" name="A" caption="make test image">
      <Subcolumn vislevel="3">
        <Row popup="false" name="A1">
          <Rhs vislevel="1" flags="1">
            <iImage window_x="2" window_y="56" window_width="526" window_height="727" image_left="4096" image_top="5200" image_mag="-16" show_status="true" show_paintbox="false" show_convert="false" show_rulers="false" scale="1" offset="0" falsecolour="false" type="true"/>
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
            <iImage window_x="2" window_y="56" window_width="928" window_height="727" image_left="914" image_top="650" image_mag="-2" show_status="true" show_paintbox="false" show_convert="false" show_rulers="false" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Image_number_format_item.S32_item.action A12"/>
          </Rhs>
        </Row>
        <Row popup="false" name="A13">
          <Rhs vislevel="1" flags="1">
            <iRegion image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true" left="104" top="80" width="146" height="162">
              <iRegiongroup/>
            </iRegion>
            <Subcolumn vislevel="0"/>
            <iText formula="Region A4 4018 3668 2184 1694"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="1202" y="0" open="true" selected="false" sform="false" next="6" name="B" caption="test max">
      <Subcolumn vislevel="3">
        <Row popup="false" name="B1">
          <Rhs vislevel="2" flags="5">
            <iText formula="Vector (map max (bandsplit A4))"/>
            <Vector/>
            <Subcolumn vislevel="0"/>
          </Rhs>
        </Row>
        <Row popup="false" name="B2">
          <Rhs vislevel="2" flags="5">
            <iText formula="Vector (map max (bandsplit A13))"/>
            <Vector/>
            <Subcolumn vislevel="0"/>
          </Rhs>
        </Row>
        <Row popup="false" name="B3">
          <Rhs vislevel="2" flags="5">
            <Vector/>
            <Subcolumn vislevel="0"/>
            <iText formula="B2 &lt;= B1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="B4">
          <Rhs vislevel="1" flags="4">
            <iText formula="min B3"/>
          </Rhs>
        </Row>
        <Row popup="false" name="B5">
          <Rhs vislevel="1" flags="4">
            <iText formula="if B4 == 0 then error &quot;max is broken!&quot; else &quot;ok&quot;"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="1202" y="276" open="true" selected="false" sform="false" next="6" name="C" caption="test min">
      <Subcolumn vislevel="3">
        <Row popup="false" name="C1">
          <Rhs vislevel="2" flags="5">
            <iText formula="Vector (map min (bandsplit A4))"/>
            <Vector/>
            <Subcolumn vislevel="0"/>
          </Rhs>
        </Row>
        <Row popup="false" name="C2">
          <Rhs vislevel="2" flags="5">
            <iText formula="Vector (map min (bandsplit A13))"/>
            <Vector/>
            <Subcolumn vislevel="0"/>
          </Rhs>
        </Row>
        <Row popup="false" name="C3">
          <Rhs vislevel="2" flags="5">
            <Vector/>
            <Subcolumn vislevel="0"/>
            <iText formula="C2 &gt;= C1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="C4">
          <Rhs vislevel="1" flags="4">
            <iText formula="min C3"/>
          </Rhs>
        </Row>
        <Row popup="false" name="C5">
          <Rhs vislevel="1" flags="4">
            <iText formula="if C4 == 0 then error &quot;max is broken!&quot; else &quot;ok&quot;"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="468" y="0" open="true" selected="true" sform="false" next="22" name="D" caption="test stats">
      <Subcolumn vislevel="3">
        <Row popup="false" name="D20">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="A4"/>
          </Rhs>
        </Row>
        <Row popup="false" name="D21">
          <Rhs vislevel="1" flags="1">
            <iRegion image_left="0" image_top="0" image_mag="0" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="0" offset="0" falsecolour="false" type="true">
              <iRegiongroup/>
            </iRegion>
            <Subcolumn vislevel="0"/>
            <iText formula="A13"/>
          </Rhs>
        </Row>
        <Row popup="false" name="D9">
          <Rhs vislevel="2" flags="5">
            <Matrix/>
            <Subcolumn vislevel="0"/>
            <iText formula="Math_stats_item.Stats_item.action D20"/>
          </Rhs>
        </Row>
        <Row popup="false" name="D10">
          <Rhs vislevel="2" flags="5">
            <Matrix/>
            <Subcolumn vislevel="0"/>
            <iText formula="Math_stats_item.Stats_item.action D21"/>
          </Rhs>
        </Row>
        <Row popup="false" name="D11">
          <Rhs vislevel="3" flags="7">
            <iText formula="Vector (transpose (D9.extract 0 0 1 49).value)?0"/>
            <Subcolumn vislevel="1"/>
            <Vector/>
          </Rhs>
        </Row>
        <Row popup="false" name="D12">
          <Rhs vislevel="3" flags="7">
            <iText formula="Vector (transpose (D10.extract 0 0 1 49).value)?0"/>
            <Subcolumn vislevel="1"/>
            <Vector/>
          </Rhs>
        </Row>
        <Row popup="false" name="D13">
          <Rhs vislevel="2" flags="5">
            <Subcolumn vislevel="0"/>
            <iText formula="D12 &gt;= D11"/>
            <Vector/>
          </Rhs>
        </Row>
        <Row popup="false" name="D14">
          <Rhs vislevel="1" flags="4">
            <iText formula="min D13"/>
          </Rhs>
        </Row>
        <Row popup="false" name="D15">
          <Rhs vislevel="3" flags="7">
            <iText formula="Vector (transpose (D9.extract 1 0 1 49).value)?0"/>
            <Subcolumn vislevel="1"/>
            <Vector/>
          </Rhs>
        </Row>
        <Row popup="false" name="D16">
          <Rhs vislevel="3" flags="7">
            <iText formula="Vector (transpose (D10.extract 1 0 1 49).value)?0"/>
            <Subcolumn vislevel="1"/>
            <Vector/>
          </Rhs>
        </Row>
        <Row popup="false" name="D17">
          <Rhs vislevel="2" flags="5">
            <Subcolumn vislevel="0"/>
            <iText formula="D16 &lt;= D15"/>
            <Vector/>
          </Rhs>
        </Row>
        <Row popup="false" name="D18">
          <Rhs vislevel="1" flags="4">
            <iText formula="min D17"/>
          </Rhs>
        </Row>
        <Row popup="false" name="D19">
          <Rhs vislevel="1" flags="4">
            <iText formula="if D14 != 255 || D18 != 255 then error &quot;VipsStats failed!&quot; else &quot;ok&quot;"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
  </Workspace>
</root>



