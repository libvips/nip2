<?xml version="1.0"?>
<root xmlns="http://www.vips.ecs.soton.ac.uk/nip/8.4.0">
  <Workspace window_x="0" window_y="0" window_width="1019" window_height="625" filename="$HOME/GIT/nip2/share/nip2/data/examples/clone/clone.ws" view="WORKSPACE_MODE_REGULAR" scale="1" offset="0" lpane_position="400" lpane_open="false" rpane_position="100" rpane_open="false" local_defs="// private definitions for this workspace&#10;" name="clone" caption="Default empty workspace">
    <Column x="0" y="0" open="true" selected="false" sform="false" next="4" name="A" caption="Images">
      <Subcolumn vislevel="3">
        <Row popup="false" name="A2">
          <Rhs vislevel="1" flags="1">
            <iImage window_x="225" window_y="466" window_width="1108" window_height="532" image_left="305" image_top="70" image_mag="4" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="-1" offset="-1" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Image_file &quot;$HOME/GIT/nip2/share/nip2/data/examples/clone/example_im_01.png&quot;"/>
          </Rhs>
        </Row>
        <Row popup="false" name="A3">
          <Rhs vislevel="1" flags="1">
            <iImage window_x="0" window_y="0" window_width="478" window_height="526" image_left="58" image_top="57" image_mag="4" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="-1" offset="-1" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Image_file &quot;$HOME/GIT/nip2/share/nip2/data/examples/clone/example_im_02.png&quot;"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="441" y="0" open="true" selected="true" sform="false" next="2" name="B" caption="Clone process">
      <Subcolumn vislevel="3">
        <Row popup="false" name="B1">
          <Rhs vislevel="3" flags="7">
            <iImage window_x="0" window_y="0" window_width="639" window_height="532" image_left="364" image_top="90" image_mag="4" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="-1" offset="-1" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1">
              <Row name="im1">
                <Rhs vislevel="0" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="im2">
                <Rhs vislevel="0" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="super">
                <Rhs vislevel="0" flags="4">
                  <iImage image_left="-1" image_top="-1" image_mag="-1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="-1" offset="-1" falsecolour="false" type="true"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="r1">
                <Rhs vislevel="1" flags="1">
                  <iRegion image_left="-1" image_top="-1" image_mag="-1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="-1" offset="-1" falsecolour="false" type="true" left="406" top="49" width="28" height="37">
                    <iRegiongroup/>
                  </iRegion>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="p2">
                <Rhs vislevel="1" flags="1">
                  <iArrow left="86" top="23" width="0" height="0">
                    <iRegiongroup/>
                  </iArrow>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="mask">
                <Rhs vislevel="1" flags="1">
                  <iImage image_left="-1" image_top="-1" image_mag="-1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="-1" offset="-1" falsecolour="false" type="true"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="Options">
                <Rhs vislevel="3" flags="6">
                  <Subcolumn vislevel="2">
                    <Row name="super">
                      <Rhs vislevel="0" flags="4">
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="pause">
                      <Rhs vislevel="1" flags="1">
                        <Toggle caption="Pause process" value="true"/>
                        <Subcolumn vislevel="0"/>
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="replace">
                      <Rhs vislevel="1" flags="1">
                        <Option/>
                        <Subcolumn vislevel="0"/>
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="balance">
                      <Rhs vislevel="1" flags="1">
                        <Toggle/>
                        <Subcolumn vislevel="0"/>
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="process">
                      <Rhs vislevel="1" flags="1">
                        <Toggle caption="Replace area with Gaussian noise." value="false"/>
                        <Subcolumn vislevel="0"/>
                        <iText/>
                      </Rhs>
                    </Row>
                    <Row name="sc">
                      <Rhs vislevel="1" flags="1">
                        <Slider/>
                        <Subcolumn vislevel="0"/>
                        <iText/>
                      </Rhs>
                    </Row>
                  </Subcolumn>
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Tasks_mosaic_item.Clone_area_item.action A2 A3"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
  </Workspace>
</root>



