<?xml version="1.0"?>
<root xmlns="http://www.vips.ecs.soton.ac.uk/nip/7.9.6">
  <Workspace name="Print_test" caption="Default empty workspace" filename="Print_test.ws" view="WORKSPACE_MODE_REGULAR">
    <Column name="A" caption="untitled" x="0" y="12" open="true" selected="false" sform="false" next="6">
      <Subcolumn vislevel="3">
        <Row name="A1" popup="false">
          <Rhs vislevel="1" flags="1">
            <iImage name="Image" caption="print_test_image.v, IM_CODING_LABQ, 549x792 8-bit unsigned integer pixels, 4 bands, Lab" window_x="182" window_y="366" window_width="565" window_height="650" image_left="284" image_top="497" image_mag="1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="1" offset="0"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Image_file &quot;$VIPSHOME/share/nip2/data/print_test_image.v&quot;"/>
          </Rhs>
        </Row>
        <Row name="A5" popup="false">
          <Rhs vislevel="1" flags="1">
            <iImage name="Image" caption="Image, 549x792 32-bit float pixels, 3 bands, Lab" window_x="-1" window_y="-1" window_width="-1" window_height="-1" image_left="-1" image_top="-1" image_mag="-1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="-1" offset="-1"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Convert_colour_to.Lab A1"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column name="B" caption="untitled" x="729" y="0" open="true" selected="false" sform="false" next="4">
      <Subcolumn vislevel="3">
        <Row name="B1" popup="false">
          <Rhs vislevel="1" flags="1">
            <iRegion name="Region" caption="Region on A5 at (0, 0), size (549, 366)" window_x="-1" window_y="-1" window_width="-1" window_height="-1" image_left="-1" image_top="-1" image_mag="-1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="-1" offset="-1" left="0" top="0" width="549" height="366">
              <iRegiongroup/>
            </iRegion>
            <Subcolumn vislevel="0">
              <Row name="image">
                <Rhs vislevel="0" flags="4">
                  <iText formula="A5"/>
                </Rhs>
              </Row>
              <Row name="left">
                <Rhs vislevel="0" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="top">
                <Rhs vislevel="0" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="width">
                <Rhs vislevel="0" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="height">
                <Rhs vislevel="0" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="super">
                <Rhs vislevel="0" flags="4">
                  <iImage name="Image" caption="Image, 549x366 32-bit float pixels, 3 bands, Lab" window_x="-1" window_y="-1" window_width="-1" window_height="-1" image_left="-1" image_top="-1" image_mag="-1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="-1" offset="-1"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="region_rect">
                <Rhs vislevel="1" flags="4">
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="value">
                <Rhs vislevel="1" flags="4">
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Region A1 42 89 136 264"/>
          </Rhs>
        </Row>
        <Row name="B2" popup="false">
          <Rhs vislevel="1" flags="1">
            <iRegion name="Region" caption="Region on A5 at (33, 411), size (493, 351)" window_x="-1" window_y="-1" window_width="-1" window_height="-1" image_left="-1" image_top="-1" image_mag="-1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="-1" offset="-1" left="33" top="411" width="493" height="351">
              <iRegiongroup/>
            </iRegion>
            <Subcolumn vislevel="0">
              <Row name="image">
                <Rhs vislevel="0" flags="4">
                  <iText formula="A5"/>
                </Rhs>
              </Row>
              <Row name="left">
                <Rhs vislevel="0" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="top">
                <Rhs vislevel="0" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="width">
                <Rhs vislevel="0" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="height">
                <Rhs vislevel="0" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="super">
                <Rhs vislevel="0" flags="4">
                  <iImage name="Image" caption="Image, 493x351 32-bit float pixels, 3 bands, Lab" window_x="-1" window_y="-1" window_width="-1" window_height="-1" image_left="-1" image_top="-1" image_mag="-1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="-1" offset="-1"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="region_rect">
                <Rhs vislevel="1" flags="4">
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="value">
                <Rhs vislevel="1" flags="4">
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Region A1 293 341 228 298"/>
          </Rhs>
        </Row>
        <Row name="B3" popup="false">
          <Rhs vislevel="1" flags="1">
            <Subcolumn vislevel="0"/>
            <iText formula="Group [B1,B2]"/>
            <Group name="Group" caption="Group [Region (Image &lt;549x792 32-bit float pixels, 3 bands&gt;) 0 0 549 366, Region (Image &lt;549x792 32-bit float pixels..."/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column name="E" caption="exercise ICC export/import" x="0" y="316" open="true" selected="false" sform="false" next="10">
      <Subcolumn vislevel="3">
        <Row name="E1" popup="false">
          <Rhs vislevel="1" flags="1">
            <iImage name="Image" caption="Image, 549x792 32-bit float pixels, 3 bands, Lab" window_x="-1" window_y="-1" window_width="-1" window_height="-1" image_left="-1" image_top="-1" image_mag="-1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="-1" offset="-1"/>
            <Subcolumn vislevel="0"/>
            <iText formula="A5"/>
          </Rhs>
        </Row>
        <Row name="E2" popup="false">
          <Rhs vislevel="3" flags="7">
            <iImage name="Image" caption="ICC_export, 549x792 8-bit unsigned integer pixels, 4 bands, CMYK" window_x="-1" window_y="-1" window_width="-1" window_height="-1" image_left="-1" image_top="-1" image_mag="-1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="-1" offset="-1"/>
            <Subcolumn vislevel="1"/>
            <iText formula="ICC_export E1"/>
          </Rhs>
        </Row>
        <Row name="E3" popup="false">
          <Rhs vislevel="3" flags="7">
            <iImage name="Image" caption="ICC_import, 549x792 32-bit float pixels, 3 bands, Lab" window_x="-1" window_y="-1" window_width="-1" window_height="-1" image_left="-1" image_top="-1" image_mag="-1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="-1" offset="-1"/>
            <Subcolumn vislevel="1"/>
            <iText formula="ICC_import E2"/>
          </Rhs>
        </Row>
        <Row name="E5" popup="false">
          <Rhs vislevel="1" flags="1">
            <iImage name="Image" caption="Image, 549x792 8-bit unsigned integer pixels, 3 bands, sRGB" window_x="-1" window_y="-1" window_width="-1" window_height="-1" image_left="-1" image_top="-1" image_mag="-1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="-1" offset="-1"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Convert_colour_to.sRGB E3"/>
          </Rhs>
        </Row>
        <Row name="E6" popup="false">
          <Rhs vislevel="3" flags="7">
            <iImage name="Image" caption="ICC_transform, 549x792 8-bit unsigned integer pixels, 4 bands, CMYK" window_x="-1" window_y="-1" window_width="-1" window_height="-1" image_left="-1" image_top="-1" image_mag="-1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="-1" offset="-1"/>
            <Subcolumn vislevel="1"/>
            <iText formula="ICC_transform E5"/>
          </Rhs>
        </Row>
        <Row name="E7" popup="false">
          <Rhs vislevel="3" flags="7">
            <iImage name="Image" caption="ICC_import, 549x792 32-bit float pixels, 3 bands, Lab" window_x="-1" window_y="-1" window_width="-1" window_height="-1" image_left="-1" image_top="-1" image_mag="-1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="-1" offset="-1"/>
            <Subcolumn vislevel="1"/>
            <iText formula="ICC_import E6"/>
          </Rhs>
        </Row>
        <Row name="E8" popup="false">
          <Rhs vislevel="3" flags="7">
            <iImage name="Image" caption="ICC_ac2rc, 549x792 32-bit float pixels, 3 bands, Lab" window_x="-1" window_y="-1" window_width="-1" window_height="-1" image_left="-1" image_top="-1" image_mag="-1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="-1" offset="-1"/>
            <Subcolumn vislevel="1"/>
            <iText formula="ICC_ac2rc E7"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
  </Workspace>
</root>



