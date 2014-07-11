#define CRCPOLY_LE 0xedb88320
#include "string.h"
#include "jcprotocol.h"
unsigned long crc32_le(unsigned long crc, unsigned char const *p, long len)
{
    int i;
    while (len--)
    {
        crc ^= *p++;
        for (i = 0; i < 8; i++)
            crc = (crc >> 1) ^ ((crc & 1) ? CRCPOLY_LE : 0);
    }
    return crc;
}

//´ò°üº¯Êý
unsigned long datapack(unsigned char *in, unsigned long len,unsigned char *out)
{

    unsigned short *hdr;
//	unsigned long  *pcrc;
    unsigned char  *pscrc;
    unsigned char  *ptr=out;
    unsigned long  i;
    unsigned long  fcs = ~(crc32_le(~0, in, len));

    *ptr++ = 0x55;
    *ptr++ = 0xAA;

    hdr=(unsigned short *)ptr;
    ptr+=2;

    *ptr++ = 0x7e;
    *ptr++ = 0x7e;
    for (i=0; i<len; i++)
    {
        switch (in[i])
        {
        case 0x7d:
            *ptr++=0x7d;
            *ptr++=0x5d;
            break;
        case 0x7e:
            *ptr++=0x7d;
            *ptr++=0x5e;
            break;
        default:
            *ptr++=in[i];
            break;
        }
    }
    pscrc=(unsigned char *)&fcs;
    for (i=0; i<4; i++,pscrc++)
    {
        switch (*pscrc)
        {
        case 0x7d:
            *ptr++=0x7d;
            *ptr++=0x5d;
            break;
        case 0x7e:
            *ptr++=0x7d;
            *ptr++=0x5e;
            break;
        default:
            *ptr++=*pscrc;
            break;
        }
    }
    *ptr++=0x7e;
    *ptr++=0x7e;
    *hdr=ptr-out-4;

    return ptr-out;
}


enum
{
    OUTSIDE_FRAME,
    LINK_ESCAPE,
    INSIDE_FRAME
};
enum FirChars
{
    JC_CE   = 0x7d,
    JC_EOF  = 0x7e,
};

#define JC_TRANS 0x20

unsigned long dataunpack(unsigned char *in, unsigned long inlen,unsigned char *out)
{
    int state=OUTSIDE_FRAME;
    int	i;
    unsigned char *data=out,*crcinpkt;
    unsigned long len=0,crc32=0,*crctmp;
    unsigned char	byte;
    //unsigned short *plen;
    unsigned short nlen;
    if (*in!=0x55 && *(in+1)!=0xaa)
    {
        return 0;
    }
    in+=2;
    //plen=(unsigned short *)in;
    memcpy(&nlen,in,sizeof(unsigned short));
    in+=2;
    //for (i = 0; i < *plen; i++)
    for (i = 0; i < nlen; i++)
    {
        byte = in[i];

        switch(state)
        {
        case OUTSIDE_FRAME:
            if (byte == JC_EOF)
                continue;
            state = INSIDE_FRAME;

        case INSIDE_FRAME:
            switch(byte)
            {
            case JC_CE:
                state = LINK_ESCAPE;
                continue;
            case JC_EOF:
                //if(i!=(*plen)-2)
                if(i!=(nlen)-2)
                    return 0;
                else
                    state=OUTSIDE_FRAME;
                continue;
            }
            break;
        case LINK_ESCAPE:
            if (byte == 0x5e)
            {
                state = INSIDE_FRAME;
                byte ^= JC_TRANS;
                break;
            }
            if (byte == 0x5d)
            {
                state = INSIDE_FRAME;
                byte ^= JC_TRANS;
                break;
            }
            goto frame_error;
        }
		
        data[len++] = byte;
		
        continue;
        
	
frame_error:
	return 0;
    }

    crcinpkt=(data+len-4);
    crc32=(~crc32_le(~0,data,len-4));
    crctmp=&crc32;
    if(memcmp(crctmp,crcinpkt,4)==0)
        return len-4;
return len;
}
