

void ReverseHexString(u8 *str, int size)
{
	u8 *tmp;
	int i, n;
	tmp = malloc(size);

	n = size - 2;
	for (i = 0; i < size; i += 2)
	{
		tmp[i + 0] = str[n + 0];
		tmp[i + 1] = str[n + 1];
		n -= 2;
	}

	for (i = 0; i < size; i++)
	{
		str[i] = tmp[i];
	}

	free(tmp);
}

void StrToHex(u8 *src, u8 *dst, int size)
{
	int i, n;
	u8 b0, b1, b2;

	n = 0;
	for (i = 0; i < size; i++)
	{
		b0 = src[i];

		b2 = b0;
		b1 = b0 / 0x10;
		b2 = b2 - (b1 * 0x10);

		b1 += 0x30;
		b2 += 0x30;

		if (b1 > 0x39) { b1 += 0x07; }
		if (b2 > 0x39) { b2 += 0x07; }

		dst[n] = b1;
		dst[n + 1] = b2;
		n += 2;
		dst[n] = 0x00;
	}
}

void ReplaceString(char *str, char *fnd, char *rpl)
{
	char tmp[20480], *tfn;
	int i, n, szFind, szRep, szFinal;

	szFind = strlen(fnd);
	szRep = strlen(rpl);
	tfn = malloc(szFind);

	n = 0;
	for (i = 0; i < strlen(str); i++)
	{
		GetStringBit(str, tfn, i, szFind);

		if ( IsEqual(tfn, fnd) )
		{
			CopyString(rpl, tmp, n);
			i += (szFind - 1);
			n += szRep;
		}
		else
		{
			tmp[n] = str[i];
			n++;
		}
		tmp[n] = 0x00;
	}

	CopyString(tmp, str, 0);

	free(tfn);
}

void GetStringBit(char *src, char *dst, int index, int size)
{
	int i;
	for (i = 0; i < size; i++)
	{
		dst[i] = src[index + i];
		dst[i + 1] = 0x00;
	}
}

void CopyString(char *src, char *dst, int index)
{
	int i;
	for (i = 0; i < strlen(src); i++)
	{
		dst[index + i] = src[i];
		dst[index + (i + 1)] = 0x00;
	}
}

int IsEqual(char *strA, char *strB)
{
	int i;

	if (strlen(strA) != strlen(strB))
	{
		return 0;
	}

	for (i = 0; i < strlen(strA); i++)
	{
		if (strA[i] != strB[i])
		{
			return 0;
		}
	}

	return 1;
}

