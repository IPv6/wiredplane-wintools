/******************************************************************************/
/*                         Start of LZRW1.C                                   */
/******************************************************************************/

/*
THE LZRW1 ALGORITHM
===================
Author : Ross N. Williams.
Date   : 31-Mar-1991.

1. I typed the following code in from my paper "An Extremely Fast Data
Compression Algorithm", Data Compression Conference, Utah, 7-11 April,
1991. The  fact that this  code works indicates  that the code  in the
paper is OK.

2. This file has been copied into a test harness and works.

3. Some users running old C compilers may wish to insert blanks around
the "="  symbols of  assignments so  as to  avoid expressions  such as
"a=*b;" being interpreted as "a=a*b;"

4. This code is public domain.

5. Warning:  This code  is non - deterministic insofar  as it  may yield
different  compressed representations  of the  same file on different
runs. (However, it will always decompress correctly to the original).

6. If you use this code in anger (e.g. in a product) drop me a note at
ross@spam.ua.oz.au and I will put you  on a mailing list which will be
invoked if anyone finds a bug in this code.

7.   The  internet   newsgroup  comp.compression   might  also   carry
information on this algorithm from time to time.

*****************************************************************************/


#define FLAG_BYTES    4     /* Number of bytes used by copy flag. */
#define FLAG_COMPRESS 0     /* Signals that compression occurred. */
#define FLAG_COPY     1     /* Signals that a copyover occurred.  */

/******************************************************************************/

dword lzrw_compress(const byte * p_src_first, dword src_len, byte * p_dst_first)
/* Input  : Specify input block using p_src_first and src_len.          */
/* Input  : Point p_dst_first to the start of the output zone (OZ).     */
/* Input  : Point p_dst_len to a dword to receive the output length.    */
/* Input  : Input block and output zone must not overlap.               */
/* Output : Length of output block written to *p_dst_len.               */
/* Output : Output block in Mem[p_dst_first..p_dst_first+*p_dst_len-1]. */
/* Output : May write in OZ=Mem[p_dst_first..p_dst_first+src_len+256-1].*/
/* Output : Upon completion guaranteed *p_dst_len<=src_len+FLAG_BYTES.  */
{

#define PS (*p++ != *s++) /* Body of inner unrolled matching loop.         */
#define ITEMMAX 16        /* Maximum number of bytes in an expanded item.  */

 const byte *p_src = p_src_first;
       byte *p_dst = p_dst_first;
 const byte *p_src_post = p_src_first + src_len;
       byte *p_dst_post = p_dst_first + src_len;
 const byte *p_src_max1 = p_src_post - ITEMMAX;
 const byte *p_src_max16 = p_src_post - 16 * ITEMMAX;
 const byte *hash[4096];
 byte *p_control; 
 word control = 0;
 word control_bits = 0;
 
 *p_dst = FLAG_COMPRESS; 
 p_dst += FLAG_BYTES; 
 p_control = p_dst; 
 p_dst+=2;
 
 while (true)
 {
    const byte *p,*s; 
    word unroll = 16;
    word len;
    word index; 
    dword offset;

    if (p_dst > p_dst_post) 
      goto overrun;
    if (p_src > p_src_max16)
    {
       unroll = 1;
       if (p_src > p_src_max1)
       {
         if (p_src == p_src_post) break; 
         goto literal;
       }
    }
    begin_unrolled_loop:
       index = ((40543*((((p_src[0] << 4) ^ p_src[1]) << 4) ^ p_src[2])) >> 4) & 0xFFF;
       p = hash[index]; 
       hash[index] = s = p_src; 
       offset = s - p;
       if ( offset > 4095 || p < p_src_first || offset == 0 || PS || PS || PS)
       {
    literal: 
         *p_dst++ = *p_src++; 
         control >>= 1; 
         control_bits++;
       }
       else
       {
          PS || PS || PS || PS || PS || PS || PS ||
          PS || PS || PS || PS || PS || PS || s++; 
          len = s - p_src - 1;
          *p_dst ++= (byte) (((offset & 0xF00) >> 4) + ( len - 1 )); 
          *p_dst ++= (byte) (offset & 0xFF);
          p_src += len; 
          control = (control >> 1) | 0x8000; 
          control_bits++;
       }
    //end_unrolled_loop
       if (--unroll) 
         goto begin_unrolled_loop;
       if (control_bits == 16)
       {
         *p_control = control & 0xFF; 
         *(p_control + 1) = control >> 8;
         p_control = p_dst; 
         p_dst += 2; 
         control = control_bits = 0;
       }
   }
   control >>= 16 - control_bits;
   *p_control++ = control & 0xFF; 
   *p_control++ = control >> 8;
   if (p_control == p_dst) 
     p_dst-=2;

   return p_dst - p_dst_first;

overrun: 
   memcpy(p_dst_first + FLAG_BYTES, p_src_first, src_len);
   *p_dst_first = FLAG_COPY; 
   return src_len + FLAG_BYTES;
}

/******************************************************************************/

dword lzrw_decompress(const byte* p_src_first, dword src_len, byte *p_dst_first)
/* Input  : Specify input block using p_src_first and src_len.          */
/* Input  : Point p_dst_first to the start of the output zone.          */
/* Input  : Point p_dst_len to a dword to receive the output length.    */
/* Input  : Input block and output zone must not overlap. User knows    */
/* Input  : upperbound on output block length from earlier compression. */
/* Input  : In any case, maximum expansion possible is eight times.     */
/* Output : Returns length of output block written.                     */
/* WRONG! Output : Output block in Mem[p_dst_first..p_dst_first+dst_len-1].    */
/* WRONG! Output : Writes only  in Mem[p_dst_first..p_dst_first+dst_len-1].    */
{
        word controlbits = 0, control;
  const byte *p_src = p_src_first + FLAG_BYTES;
        byte *p_dst = p_dst_first;
  const byte *p_src_post = p_src_first + src_len;

  if (*p_src_first==FLAG_COPY)
  { 
    memcpy(p_dst_first, p_src_first + FLAG_BYTES, src_len - FLAG_BYTES);
    return src_len - FLAG_BYTES; 
  }
  while (p_src != p_src_post)
  {
    if (controlbits==0)
    {
      control = *p_src++; 
      control |= (*p_src++)<<8; 
      controlbits = 16;
    }
    if(control & 1)
    {
       word offset, len; byte *p;
       offset = (*p_src & 0xF0) << 4; 
       len = 1 + (*p_src++ & 0xF);
       offset += *p_src++ & 0xFF; 
       p = p_dst - offset;
       while (len--) *p_dst++ = *p++;
    }
    else
       *p_dst++ = *p_src++;
    control >>= 1; controlbits--;
  }
  return p_dst - p_dst_first;
}

namespace tool 
{
  
  namespace lzrw 
  {
    void compress(const byte* src, size_t src_length, array<byte>& dst)
    {
      dst.size(sizeof(dword) + src_length + 256 - 1);
      dword dst_size = lzrw_compress( src, src_length, dst.head() + sizeof(dword) );
      *((dword*)dst.head()) = src_length;
      dst.size(sizeof(dword) + dst_size);
    }

    void decompress(const byte* src, size_t src_length, array<byte>& dst)
    {
      dword dst_size = *((dword *)src);
      dst.size(dst_size);
      dword dst_size_1 = lzrw_decompress( src + sizeof(dword), src_length - sizeof(dword), dst.head() );
      assert(dst_size_1 == dst_size);
    }

  }

}

/******************************************************************************/
/*                          End of LZRW1.C                                    */
/******************************************************************************/


/* // Второй метод - жмет медленней но лучше в 1.5-3 раза
#define BLZ_WORKMEM_SIZE     1048576L
#define BLZ_MAX_PACK_SIZE(a) ((a) + ((a)/8) + 64)

#if BLZ_WORKMEM_SIZE & (BLZ_WORKMEM_SIZE-1)
#  error BLZ_WORKMEM_SIZE must be a power of 2
#endif

long blz_enpack (const unsigned char *, unsigned char *, long, unsigned char*);
long blz_depack (const unsigned char *, unsigned char *, long);
long blz_crc32  (const unsigned char *, long, long);

const unsigned long blz_crctab_n[16] = {
   0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac, 0x76dc4190,
   0x6b6b51f4, 0x4db26158, 0x5005713c, 0xedb88320, 0xf00f9344,
   0xd6d6a3e8, 0xcb61b38c, 0x9b64c2b0, 0x86d3d2d4, 0xa00ae278,
   0xbdbdf21c
};

typedef struct {
   const unsigned char *source;
   unsigned char       *destination;
   unsigned char       *tagpos;
   unsigned long        tag;
   unsigned long        bitcount;
} BLZPACKDATA;

static void blz_putbit(BLZPACKDATA *ud, const long bit){
  if(!ud->bitcount--){
    ud->tagpos[0] = (unsigned char)(ud->tag & 0x00ff);
    ud->tagpos[1] = (unsigned char)((ud->tag >> 8) & 0x00ff);

    ud->tagpos       = ud->destination;
    ud->destination += 2;
    ud->bitcount     = 15;
  }
  ud->tag = (ud->tag << 1) + (bit ? 1 : 0);
}

static void blz_putgamma(BLZPACKDATA *ud, unsigned long val){
  unsigned long mask = val >> 1;

  while(mask & (mask - 1)) mask &= mask - 1;

  blz_putbit(ud, val & mask);

  while(mask >>= 1){
    blz_putbit(ud, 1);
    blz_putbit(ud, val & mask);
  }
  blz_putbit(ud, 0);
}

static unsigned long blz_hash4(const unsigned char *data){
  unsigned long val = data[0];
  val = (val*317) + data[1];
  val = (val*317) + data[2];
  val = (val*317) + data[3];
  return (val & (BLZ_WORKMEM_SIZE/4 - 1));
}

long blz_enpack(const unsigned char * source
                    , unsigned char * destination
                    ,          long   length
                    , unsigned char * workmem
){
  BLZPACKDATA ud;
  const unsigned char **lookup = (const unsigned char **)workmem;
  const unsigned char *backptr = source;

  if(length == 0) return 0;

  {
    long i;
    for (i = 0; i < BLZ_WORKMEM_SIZE/4; ++i) lookup[i] = 0;
  }

  ud.source      = (const unsigned char*)source;
  ud.destination = (      unsigned char*)destination;

  *ud.destination++ = *ud.source++;

  if(--length == 0)return 1;

  ud.tagpos       = ud.destination;
  ud.destination += 2;
  ud.tag          = 0;
  ud.bitcount     = 16;

  while(length > 4){
    const unsigned char *ppos;
    long len = 0;

    while(backptr < ud.source){
      lookup[blz_hash4(backptr)] = backptr;
      backptr++;
    }

    ppos = lookup[blz_hash4(ud.source)];

    if(ppos){
      while((len < length) && (*(ppos + len) == *(ud.source + len))) ++len;
    }

    if(len > 3){
      unsigned long pos(ud.source - ppos - 1);

      blz_putbit(&ud, 1);

      blz_putgamma(&ud, len - 2);

      blz_putgamma(&ud, (pos >> 8) + 2);
      *ud.destination++ = (unsigned char)(pos & 0x00ff);

      ud.source += len;
      length -= len;
    }else{
      blz_putbit(&ud, 0);

      *ud.destination++ = *ud.source++;
      length--;
    }
  }

  while(length > 0){
    blz_putbit(&ud, 0);

    *ud.destination++ = *ud.source++;
    length--;
  }

  ud.tag     <<= ud.bitcount;
  ud.tagpos[0] = (unsigned char)(ud.tag & 0x00ff);
  ud.tagpos[1] = (unsigned char)((ud.tag >> 8) & 0x00ff);

  return long(ud.destination - (unsigned char *)destination);
}

typedef struct {
   const unsigned char *source;
   unsigned char       *destination;
   unsigned long        tag;
   unsigned long        bitcount;
} BLZDEPACKDATA;

static long blz_getbit(BLZDEPACKDATA *ud){
  unsigned long bit;

  if(!ud->bitcount--){
    ud->tag      = ud->source[0] + ((unsigned long)ud->source[1] << 8);
    ud->source  += 2;
    ud->bitcount = 15;
  }

  bit       = (ud->tag >> 15) & 0x01;
  ud->tag <<= 1;

  return bit;
}

static unsigned long blz_getgamma(BLZDEPACKDATA *ud){
  unsigned long result = 1;

  do{
    result = (result << 1) + blz_getbit(ud);
  }while(blz_getbit(ud));

  return (result);
}

long blz_depack(const unsigned char * source
                    , unsigned char * destination
                    ,          long   depacked_length
){
  BLZDEPACKDATA ud;
  long length = 1;

  if(depacked_length == 0) return 0;

  ud.source      = source;
  ud.destination = destination;
  ud.bitcount    = 0;

  *ud.destination++ = *ud.source++;

  while(length < depacked_length){
    if(blz_getbit(&ud)){
      unsigned long len = blz_getgamma(&ud) + 2;
      unsigned long pos = blz_getgamma(&ud) - 2;

      pos = (pos << 8) + *ud.source++ + 1;

      {
        const unsigned char *ppos = ud.destination - pos;
        long i;
        for(i = len; i > 0; --i) *ud.destination++ = *ppos++;
      }

      length += len;

    }else{
      *ud.destination++ = *ud.source++;

      length++;
    }
  }
  return length;
}
// пример
//  unsigned char *src = new unsigned char [size];
//  unsigned char *buf = new unsigned char [BLZ_MAX_PACK_SIZE(size)];
//  unsigned char *mem = new unsigned char [BLZ_WORKMEM_SIZE];
//  long osize = blz_enpack (src, buf, size, mem);
//  std::cout << osize << std::endl;
*/