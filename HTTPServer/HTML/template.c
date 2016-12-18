char *template_html = "
<HTML>
<HEAD>
<LINK REL=STYLESHEET HREF='CLounge.css' TYPE='text/css'>
</HEAD>
<TITLE>CheatersLounge</TITLE>
<BODY>

<table style='position:absolute; top:0px; left:0px;' border=0 cellspacing=0 cellpadding=0 width=100% height=100%>

<!-- Banner -->
<tr height='29px'>
	<td background='/headergrunge.jpg'>
		<!-- Space -->
	</td>
</tr>

<!-- CheatersLounge Logo -->
<tr height='100px'>
	<td>
		<center><img src='/CLoungePlain.jpg'></center>
	</td>
</tr>

<!-- Body -->
<TR HEIGHT=100%>
	<TD WIDTH=100%>
		<!-- Middle Body -->
		<CENTER>

		<TABLE BORDER=0 CELLSPACING=3 CELLPADDING=3 WIDTH=100% HEIGHT=100%>
		<TR HEIGHT=100%>
			<TD>
				<!-- Something could go here... -->
			</TD>
			<TD><CENTER>
				<TABLE BORDER=0 CELLSPACING=3 CELLPADDING=3 WIDTH=100%>
				<TR>
					<TD>
						<CENTER>
						<br />
						RemotePS2 (HTTP Server Edition)
						<br />
						<br />
						Current build is not final!
						<br />
						Supports:<br />
						1. Menu browsing<br />
						2. Send Code List (view information page)<br />
						3. Reset Console (performs LoadExecPS2() with null arguments)<br />
						4. Launch Game (sets up code list and installs engine, starts game)<br />
						</CENTER>
					</TD>
				</TR>
				</TABLE>
				<br />
				<a href='/index.gt?action=none'>Home</a> | <a href='/codes.gt?action=none'>Codes</a> | <a href='/do.gt?action=reset'>Reset PS2</a> | <a href='/do.gt?action=execgame'>Launch Game Disc</a> | <a href='/info.gt'>Info.</a> | <a href='http://cheaters-lounge.net/'>CheatersLounge</a> | <a href='http://cheaters-lounge.net/index.php?p=17'>Archives</a><br />
				-----------------------------------------------------------------------------
				<br />
				<TABLE BORDER=0 CELLSPACING=3 CELLPADDING=3 WIDTH=100%>
					<TR>
						<TD WIDTH=100%><CENTER>

							$TABLEDATA

						</CENTER></TD>
					</TR>
				</TABLE>
			</CENTER></TD>
			<TD>
				<!-- And here... -->
			</TD>
		</TR>
		</TABLE>

		</CENTER>
	</TD>
</TR>

<!-- <tr height='100px'> -->
<tr>
	<td>
		<!-- Spacer -->
		<br />
	</td>
</tr>

<!-- Bottom Bar Logo -->
<tr height='36px'>
	<td>
		<center>
		Created By: Gtlcpimp<br />
		Official CheatersLounge Product<br />
		Copyright © 2010 CheatersLounge
		</center>
	</td>
</tr>

<tr height='29px'>
	<td background='/headergrunge2.jpg'>
		<!-- Space -->
	</td>
</tr>

</TABLE>

</BODY>
</HTML>

";

