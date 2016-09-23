<?xml version="1.0"?>
<root xmlns="http://www.vips.ecs.soton.ac.uk/nip/8.4.0">
  <Workspace window_x="0" window_y="0" window_width="1193" window_height="683" filename="$HOME/GIT/nip2/share/nip2/data/examples/manual_balance/manual_balance.ws" view="WORKSPACE_MODE_REGULAR" scale="1" offset="0" lpane_position="400" lpane_open="false" rpane_position="100" rpane_open="false" local_defs="// private definitions for this workspace&#10;" name="manual_balance" caption="Default empty workspace">
    <Column x="0" y="0" open="true" selected="true" sform="false" next="8" name="A" caption="Images">
      <Subcolumn vislevel="3">
        <Row popup="false" name="A1">
          <Rhs vislevel="1" flags="1">
            <iImage window_x="602" window_y="480" window_width="745" window_height="689" image_left="364" image_top="276" image_mag="1" show_status="true" show_paintbox="false" show_convert="true" show_rulers="false" scale="1" offset="0" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Image_file &quot;$HOME/GIT/nip2/share/nip2/data/examples/manual_balance/simp_base.png&quot;"/>
          </Rhs>
        </Row>
        <Row popup="false" name="A2">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="-1" image_top="-1" image_mag="-1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="-1" offset="-1" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Image_file &quot;$HOME/GIT/nip2/share/nip2/data/examples/manual_balance/mask_control.png&quot;"/>
          </Rhs>
        </Row>
        <Row popup="false" name="A3">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="-1" image_top="-1" image_mag="-1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="-1" offset="-1" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Image_file &quot;$HOME/GIT/nip2/share/nip2/data/examples/manual_balance/mask_01.png&quot;"/>
          </Rhs>
        </Row>
        <Row popup="false" name="A4">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="-1" image_top="-1" image_mag="-1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="-1" offset="-1" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Image_file &quot;$HOME/GIT/nip2/share/nip2/data/examples/manual_balance/mask_02.png&quot;"/>
          </Rhs>
        </Row>
        <Row popup="false" name="A5">
          <Rhs vislevel="1" flags="1">
            <iImage image_left="-1" image_top="-1" image_mag="-1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="-1" offset="-1" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Image_file &quot;$HOME/GIT/nip2/share/nip2/data/examples/manual_balance/mask_03.png&quot;"/>
          </Rhs>
        </Row>
        <Row popup="false" name="A6">
          <Rhs vislevel="1" flags="1">
            <Group/>
            <Subcolumn vislevel="0"/>
            <iText formula="Group [A3,A4,A5]"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
    <Column x="781" y="0" open="true" selected="false" sform="false" next="4" name="B" caption="Process">
      <Subcolumn vislevel="3">
        <Row popup="false" name="B1">
          <Rhs vislevel="1" flags="1">
            <Matrix/>
            <Subcolumn vislevel="0"/>
            <iText formula="Tasks_mosaic_item.Manual_balance_item.Balance_find_item.action A1 A2 A6"/>
          </Rhs>
        </Row>
        <Row popup="false" name="B2">
          <Rhs vislevel="3" flags="7">
            <iImage image_left="-1" image_top="-1" image_mag="-1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="-1" offset="-1" falsecolour="false" type="true"/>
            <Subcolumn vislevel="1">
              <Row name="im_in">
                <Rhs vislevel="0" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="m_matrix">
                <Rhs vislevel="1" flags="4">
                  <iText/>
                </Rhs>
              </Row>
              <Row name="m_group">
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
              <Row name="blur">
                <Rhs vislevel="1" flags="1">
                  <Slider/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="adjust">
                <Rhs vislevel="1" flags="1">
                  <Matrix/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="Build">
                <Rhs vislevel="1" flags="1">
                  <Toggle caption="Build Scale and Offset Correction Images" value="true"/>
                  <Subcolumn vislevel="0"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="Output">
                <Rhs vislevel="2" flags="6">
                  <Subcolumn vislevel="1"/>
                  <iText/>
                </Rhs>
              </Row>
              <Row name="value">
                <Rhs vislevel="1" flags="4">
                  <iText/>
                </Rhs>
              </Row>
            </Subcolumn>
            <iText formula="Tasks_mosaic_item.Manual_balance_item.Balance_check_item.action A1 B1 A6"/>
          </Rhs>
        </Row>
        <Row popup="false" name="B3">
          <Rhs vislevel="1" flags="1">
            <iImage window_x="2997" window_y="620" window_width="516" window_height="543" image_left="240" image_top="240" image_mag="1" show_status="false" show_paintbox="false" show_convert="false" show_rulers="false" scale="-1" offset="-1" falsecolour="false" type="true"/>
            <Subcolumn vislevel="0"/>
            <iText formula="Tasks_mosaic_item.Manual_balance_item.Balance_apply_item.action A1 B2.Output.scale_im B2.Output.offset_im"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
  </Workspace>
</root>



