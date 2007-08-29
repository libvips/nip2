<?xml version="1.0"?>
<root xmlns="http://www.vips.ecs.soton.ac.uk/nip/7.9.6">
  <Workspace name="test" caption="Default empty workspace" filename="test.ws" view="WORKSPACE_MODE_REGULAR">
    <Column name="D" caption="untitled" x="0" y="0" open="true" selected="true" sform="false" next="18">
      <Subcolumn vislevel="3">
        <Row name="D10" popup="false">
          <Rhs vislevel="1" flags="4">
            <iText formula="&quot;this test has an adjust_scale_offset whose scale slider has a value&quot;"/>
          </Rhs>
        </Row>
        <Row name="D11" popup="false">
          <Rhs vislevel="1" flags="4">
            <iText formula="&quot;overwritten with the image mean&quot;"/>
          </Rhs>
        </Row>
        <Row name="D12" popup="false">
          <Rhs vislevel="1" flags="4">
            <iText formula="&quot;nip2 has to work to get the recomp order right, and has to be careful&quot;"/>
          </Rhs>
        </Row>
        <Row name="D13" popup="false">
          <Rhs vislevel="1" flags="4">
            <iText formula="&quot;that computation errors discovered during backtracking are overwritten&quot;"/>
          </Rhs>
        </Row>
        <Row name="D14" popup="false">
          <Rhs vislevel="1" flags="4">
            <iText formula="&quot;correctly when the true value is known&quot;"/>
          </Rhs>
        </Row>
        <Row name="D15" popup="false">
          <Rhs vislevel="1" flags="4">
            <iText formula="&quot;try duping the column or removing D6&quot;"/>
          </Rhs>
        </Row>
        <Row name="D3" popup="false">
          <Rhs vislevel="1" flags="1">
            <iImage name="Image" caption="cd1.1.v, 531x373 8-bit unsigned integer pixels, 1 band, mono" window_x="220" window_y="176" window_width="1133" window_height="876" image_left="1245" image_top="712" image_mag="1" show_status="false" show_paintbox="false" show_convert="true" show_rulers="false" scale="0.54513" offset="0"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Image_file &quot;$VIPSHOME/share/nip2/data/IR/cd1.1.v&quot;"/>
          </Rhs>
        </Row>
        <Row name="D6" popup="false">
          <Rhs vislevel="1" flags="4">
            <iText formula="Mean D3"/>
          </Rhs>
        </Row>
        <Row name="D7" popup="false">
          <Rhs vislevel="3" flags="7">
            <iImage name="Image" caption="Adjust_scale_offset, 531x373 8-bit unsigned integer pixels, 1 band, mono" window_x="0" window_y="0" window_width="1085" window_height="957" image_left="525" image_top="447" image_mag="1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="1" offset="0"/>
            <Subcolumn vislevel="1">
              <Row name="x">
                <Rhs vislevel="0" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="super">
                <Rhs vislevel="0" flags="4">
                  <iImage name="Image" caption="Image, 531x373 8-bit unsigned integer pixels, 1 band, mono" window_x="-1" window_y="-1" window_width="-1" window_height="-1" image_left="-1" image_top="-1" image_mag="-1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="-1" offset="-1"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="scale">
                <Rhs vislevel="4" flags="7">
                  <Slider name="Slider"/>
                  <Subcolumn vislevel="2">
                    <Row name="from">
                      <Rhs vislevel="0" flags="4">
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="to">
                      <Rhs vislevel="0" flags="4">
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="value">
                      <Rhs vislevel="0" flags="4">
                        <iText formula="D6"/>
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
              <Row name="offset">
                <Rhs vislevel="1" flags="1">
                  <Slider name="Slider"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Adjust_scale_offset D3"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
  </Workspace>
</root>



