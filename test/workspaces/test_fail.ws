<?xml version="1.0"?>
<root xmlns="http://www.vips.ecs.soton.ac.uk/nip/8.5.0">
  <Workspace filename="/home/john/GIT/nip2/test/workspaces/test_fail.ws" view="WORKSPACE_MODE_REGULAR" scale="1" offset="0" window_width="680" window_height="500" lpane_position="100" lpane_open="false" rpane_position="400" rpane_open="false" local_defs="// private definitions for this workspace&#10;" name="untitled" caption="Default empty workspace">
    <Column x="0" y="0" open="true" selected="true" sform="false" next="4" name="A">
      <Subcolumn vislevel="3">
        <Row popup="false" name="A1">
          <Rhs vislevel="1" flags="4">
            <iText formula="1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="A2">
          <Rhs vislevel="1" flags="4">
            <iText formula="1"/>
          </Rhs>
        </Row>
        <Row popup="false" name="A3">
          <Rhs vislevel="1" flags="4">
            <iText formula="if A1 == A2 then error &quot;sane!&quot; else &quot;i'm mad&quot;"/>
          </Rhs>
        </Row>
      </Subcolumn>
    </Column>
  </Workspace>
</root>



