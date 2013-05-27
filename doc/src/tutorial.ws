A @ COLUMN 0 1 true untitled column
A A1 IMAGE $VIPSHOME/pics/huysum.hr.v
A A2 USER Invert A1 
A A3 USER Invert A2 
A A4 USER Equal A1 A3 
A A5 USER Min A4 
B @ COLUMN 385 0 true 
B B1 SLIDER 0 255 229.5
B B2 USER More A2 B1 
B B3 USER 2.3 
B B4 USER A1 ^ ( 1 / B7 ) 
B B5 USER B4 * ( 255 / 255 ^ ( 1 / B7 ) ) 
B B6 USER Convert_to_unsigned_char B5 
B B7 SLIDER 0 3 0.37
B B8 USER Gamma A1 B7 
B B9 REGION A1 756 1188 352 404
B B10 USER Average B9 
C @ COLUMN 708 0 true 
C C1 IMAGE /export/home/vips/vips7.5/pics/campin.v
C C2 USER 0.3 * C1 \ 1 + 0.6 * C1 \ 2 + 0.1 * C1 \ 3 
C C3 USER Convert_to_unsigned_char C2 
C C5 USER C14 C1 
C C6 USER Convert_to_unsigned_char C5 
C C8 USER C15 C3 
C C9 SLIDER 0 255 220.04
C C10 USER More C8 C9 
C C12 USER C16 C10 
C C13 USER Eor C10 C12 
J @ COLUMN 1062 0 true LR gradient correction
J J1 USER "-------------- Inputs -------------" 
J J2 USER "Image to be corrected:" 
J J3 USER A1
J J4 USER "Correction to perform:" 
J J5 SLIDER -1 1 -0.99
J J6 USER "------------- Compute -------------" 
J J7 USER Generate_fgrey J3 . w J3 . h 
J J8 USER ( J7 - 0.5 ) * J5 + 1 
J J9 USER J8 * J3 
J J10 USER "------------- Outputs ------------" 
J J11 USER "Corrected image:" 
J J12 USER clipfmt J3 . fmt J9 
