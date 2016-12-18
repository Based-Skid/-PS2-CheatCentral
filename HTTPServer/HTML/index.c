char *index_html = "

<html>
<body bgcolor=303030 text=DCDCDC>

<table border=0 cellspace=0 cellpadd=0 width=100% height=100%>
<tr>
	<td>
		<center>

		<table border=0 cellspace=3 cellpadd=3>
		<tr>
			<td>
				<a href='/index.gt'><button>Home</button></a>
			</td>
			<td>
				<a href='/codes.gt'><button>Code Page</button></a>
			</td>
			<td>
				<a href='/do.gt?action=reset'><button>Reset PS2</button></a>
			</td>
			<td>
				<a href='/do.gt?action=execgame'><button>Launch Game Disc</button></a>
			</td>
		</tr>
		</table>

		<br /><br />
		CL-PS2 HTTP-Server Debug Information
		<table border=0 cellspace=3 cellpadd=3>
		<tr>
			<td>
				$GETSIZE
			</td>
			<td>
				GET
			</td>
			<td>
				$GETDATA
			</td>
		</tr>
		<tr>
			<td>
				N/A
			</td>
			<td>
				Page URL
			</td>
			<td>
				$PAGEURL
			</td>
		</tr>
		<tr>
			<td>
				N/A
			</td>
			<td>
				Action
			</td>
			<td>
				$ACTION
			</td>
		</tr>
		</table>

		<br /><br />
		HTTP Request:<br />
		<textarea rows=20 cols=110>$RECDATA</textarea><br />
		</center>
	</td>
</tr>
<tr height=100%>
	<td width=100%>
		<!-- Spring -->
	</td>
</tr>
<tr>
	<td>
		Copyright © 2010 CheatersLounge
	</td>
</tr>
<table>
</body>
</html>
";
