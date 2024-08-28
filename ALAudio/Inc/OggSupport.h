/*------------------------------------------------------------------------------------
	Helpers to play OGG files from memory.
------------------------------------------------------------------------------------*/

size_t MEM_readOgg(void* dst, size_t size1, size_t size2, void* fh)
{
    memory_ogg_file* mof = reinterpret_cast<memory_ogg_file*>(fh);
    size_t len = size1 * size2;
    if ( mof->curPtr + len > mof->filePtr + mof->fileSize )
    {
        len = mof->filePtr + mof->fileSize - mof->curPtr;
    }
    memcpy( dst, mof->curPtr, len );
    mof->curPtr += len;
    return len;
}

int MEM_seekOgg( void *fh, ogg_int64_t to, int type ) {
    memory_ogg_file* mof = reinterpret_cast<memory_ogg_file*>(fh);

    switch( type ) {
        case SEEK_CUR:
            mof->curPtr += to;
            break;
        case SEEK_END:
            mof->curPtr = mof->filePtr + mof->fileSize - to;
            break;
        case SEEK_SET:
            mof->curPtr = mof->filePtr + to;
            break;
        default:
            return -1;
    }
    if ( mof->curPtr < mof->filePtr ) {
        mof->curPtr = mof->filePtr;
        return -1;
    }
    if ( mof->curPtr > mof->filePtr + mof->fileSize ) {
        mof->curPtr = mof->filePtr + mof->fileSize;
        return -1;
    }
    return 0;
}

int MEM_closeOgg(void* fh)
{
    return 0;
}

long MEM_tellOgg( void *fh )
{
    memory_ogg_file* mof = reinterpret_cast<memory_ogg_file*>(fh);
    return (mof->curPtr - mof->filePtr);
}
bool IsOggFormat( BYTE* Data, int DataLen )
{
	static char OGGHeaderID[]={0x4F,0x67,0x67,0x53,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	if( DataLen<sizeof(OGGHeaderID))
		return 0;
	char i=0;
	for( ; i<ARRAY_COUNT(OGGHeaderID); ++i )
		if( OGGHeaderID[i]!=Data[i] )
			return 0;
	return 1;
}
