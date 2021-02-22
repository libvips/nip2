<?xml version="1.0"?>
<root xmlns="http://www.vips.ecs.soton.ac.uk/nip/8.5.0">
  <Workspace window_x="3" window_y="84" window_width="838" window_height="871" view="WORKSPACE_MODE_REGULAR" scale="1" offset="0" locked="false" lpane_position="400" lpane_open="false" rpane_position="100" rpane_open="false" local_defs="// private definitions for this workspace&#10;" name="test" caption="Default empty workspace" filename="$HOME/GIT/nip2/test/extras/test.ws" major="7" minor="9">
    <Column x="0" y="0" open="true" selected="false" sform="false" next="21" name="D" caption="untitled">
      <Subcolumn vislevel="3">
        <Row popup="false" name="D10">
          <Rhs vislevel="1" flags="4">
            <iText formula="&quot;this test has an adjust_scale_offset whose scale slider has a value&quot;"/>
          </Rhs>
        </Row>
        <Row popup="false" name="D11">
          <Rhs vislevel="1" flags="4">
            <iText formula="&quot;overwritten with the image mean&quot;"/>
          </Rhs>
        </Row>
        <Row popup="false" name="D12">
          <Rhs vislevel="1" flags="4">
            <iText formula="&quot;nip2 has to work to get the recomp order right, and has to be careful&quot;"/>
          </Rhs>
        </Row>
        <Row popup="false" name="D13">
          <Rhs vislevel="1" flags="4">
            <iText formula="&quot;that computation errors discovered during backtracking are overwritten&quot;"/>
          </Rhs>
        </Row>
        <Row popup="false" name="D14">
          <Rhs vislevel="1" flags="4">
            <iText formula="&quot;correctly when the true value is known&quot;"/>
          </Rhs>
        </Row>
        <Row popup="false" name="D15">
          <Rhs vislevel="1" flags="4">
            <iText formula="&quot;try duping the column or removing D6&quot;"/>
          </Rhs>
        </Row>
        <Row popup="false" name="D3">
          <Rhs vislevel="1" flags="1">
            <iImage window_x="220" window_y="175" window_width="1133" window_height="876" image_left="6" image_top="5" image_mag="80" show_status="false" show_paintbox="false" show_convert="true" show_rulers="false" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Image_file &quot;$HOME/GIT/nip2/test/extras/test-5x5.v&quot;"/>
          </Rhs>
        </Row>
        <Row popup="false" name="D6">
          <Rhs vislevel="1" flags="4">
            <iText formula="Mean D3"/>
          </Rhs>
        </Row>
        <Row popup="false" name="D7">
          <Rhs vislevel="3" flags="7">
            <iImage window_x="0" window_y="0" window_width="1085" window_height="957" image_left="2" image_top="2" image_mag="184" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1">
              <Row name="super">
                <Rhs vislevel="0" flags="4">
                  <iImage image_left="-1" image_top="-1" image_mag="-1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="-1" offset="-1" falsecolour="false" type="true"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="scale">
                <Rhs vislevel="4" flags="7">
                  <Slider/>
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
                      <Rhs vislevel="1" flags="1">
                        <Subcolumn vislevel="0"/>
                        <iText/>
                        <Real/>
                      </Rhs>
                    </Row>
                    <Row name="caption">
                      <Rhs vislevel="0" flags="4">
                        <iText/>
                      </Rhs>
                    </Row>
                  </Subcolumn>
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
              <Row name="image">
                <Rhs vislevel="0" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="value">
                <Rhs vislevel="1" flags="4">
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
