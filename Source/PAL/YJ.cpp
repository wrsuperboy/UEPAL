// Copyright (C) 2022 Meizhouxuanhan.


#include "YJ.h"

typedef struct _YJ1_TreeNode
{
	uint8 value;
	uint8 leaf;
	uint16 level;
	uint32 weight;

	struct _YJ1_TreeNode* parent;
	struct _YJ1_TreeNode* left;
	struct _YJ1_TreeNode* right;
} YJ1_TreeNode;

typedef struct _YJ1_TreeNodeList
{
	YJ1_TreeNode* node;
	struct _YJ1_TreeNodeList* next;
} YJ1_TreeNodeList;

typedef struct _YJ_1_FILEHEADER
{
	uint32 Signature; // 'YJ_1'
	uint32 UncompressedLength; // size before compression
	uint32 CompressedLength; // size after compression
	uint16 BlockCount; // number of blocks
	uint8 Unknown;
	uint8 HuffmanTreeLength; // length of huffman tree
} YJ_1_FILEHEADER, * PYJ_1_FILEHEADER;

typedef struct _YJ_1_BLOCKHEADER
{
	uint16 UncompressedLength; // maximum 0x4000
	uint16 CompressedLength; // including the header
	uint16 LZSSRepeatTable[4];
	uint8 LZSSOffsetCodeLengthTable[4];
	uint8 LZSSRepeatCodeLengthTable[3];
	uint8 CodeCountCodeLengthTable[3];
	uint8 CodeCountTable[2];
} YJ_1_BLOCKHEADER, * PYJ_1_BLOCKHEADER;

static uint32
yj1_get_bits(
	const void* src,
	uint32* bitptr,
	uint32 count
)
{
	uint8* temp = ((uint8*)src) + ((*bitptr >> 4) << 1);
	uint32 bptr = *bitptr & 0xf;
	uint16 mask;
	*bitptr += count;
	if (count > 16 - bptr)
	{
		count = count + bptr - 16;
		mask = 0xffff >> bptr;
		return (((temp[0] | (temp[1] << 8)) & mask) << count) | ((temp[2] | (temp[3] << 8)) >> (16 - count));
	}
	else
		return (((uint16)((temp[0] | (temp[1] << 8)) << bptr)) >> (16 - count));
}

static uint16
yj1_get_loop(
	const void* src,
	uint32* bitptr,
	PYJ_1_BLOCKHEADER header
)
{
	if (yj1_get_bits(src, bitptr, 1))
		return header->CodeCountTable[0];
	else
	{
		uint32 temp = yj1_get_bits(src, bitptr, 2);
		if (temp)
			return yj1_get_bits(src, bitptr, header->CodeCountCodeLengthTable[temp - 1]);
		else
			return header->CodeCountTable[1];
	}
}

static uint16
yj1_get_count(
	const void* src,
	uint32* bitptr,
	PYJ_1_BLOCKHEADER header
)
{
	uint16 temp;
	if ((temp = yj1_get_bits(src, bitptr, 2)) != 0)
	{
		if (yj1_get_bits(src, bitptr, 1))
			return yj1_get_bits(src, bitptr, header->LZSSRepeatCodeLengthTable[temp - 1]);
		else
			return header->LZSSRepeatTable[temp];
	}
	else
		return header->LZSSRepeatTable[0];
}

SIZE_T
YJ1_Decompress(
	uint8* Source,
	uint8* Destination,
	SIZE_T DestSize
)
{
	PYJ_1_FILEHEADER hdr = (PYJ_1_FILEHEADER)Source;
	uint8* src = (uint8*)Source;
	uint8* dest;
	uint32 i;
	YJ1_TreeNode* root, * node;

	if (Source == nullptr)
		return -1;
	if (hdr->Signature != 0x315f4a59)
		return -1;
	if (hdr->UncompressedLength > (uint32)DestSize)
		return -1;

	do
	{
		uint16 tree_len = ((uint16)hdr->HuffmanTreeLength) * 2;
		uint32 bitptr = 0;
		uint8* flag = (uint8*)src + 16 + tree_len;

		if ((node = root = (YJ1_TreeNode*)FMemory::Malloc(sizeof(YJ1_TreeNode) * (tree_len + 1))) == nullptr)
			return -1;
		root[0].leaf = 0;
		root[0].value = 0;
		root[0].left = root + 1;
		root[0].right = root + 2;
		for (i = 1; i <= tree_len; i++)
		{
			root[i].leaf = !yj1_get_bits(flag, &bitptr, 1);
			root[i].value = src[15 + i];
			if (root[i].leaf)
				root[i].left = root[i].right = nullptr;
			else
			{
				root[i].left = root + (root[i].value << 1) + 1;
				root[i].right = root[i].left + 1;
			}
		}
		src += 16 + tree_len + (((tree_len & 0xf) ? (tree_len >> 4) + 1 : (tree_len >> 4)) << 1);
	} while (0);

	dest = (uint8*)Destination;

	for (i = 0; i < hdr->BlockCount; i++)
	{
		uint32 bitptr;
		PYJ_1_BLOCKHEADER header;

		header = (PYJ_1_BLOCKHEADER)src;
		src += 4;
		if (!header->CompressedLength)
		{
			uint16 hul = header->UncompressedLength;
			while (hul--)
			{
				*dest++ = *src++;
			}
			continue;
		}
		src += 20;
		bitptr = 0;
		for (;;)
		{
			uint16 loop;
			if ((loop = yj1_get_loop(src, &bitptr, header)) == 0)
				break;

			while (loop--)
			{
				node = root;
				for (; !node->leaf;)
				{
					if (yj1_get_bits(src, &bitptr, 1))
						node = node->right;
					else
						node = node->left;
				}
				*dest++ = node->value;
			}

			if ((loop = yj1_get_loop(src, &bitptr, header)) == 0)
				break;

			while (loop--)
			{
				uint32 pos, count;
				count = yj1_get_count(src, &bitptr, header);
				pos = yj1_get_bits(src, &bitptr, 2);
				pos = yj1_get_bits(src, &bitptr, header->LZSSOffsetCodeLengthTable[pos]);
				while (count--)
				{
					*dest = *(dest - pos);
					dest++;
				}
			}
		}
		src = ((uint8*)header) + header->CompressedLength;
	}
	FMemory::Free(root);

	return hdr->UncompressedLength;
}

/* ============================================================================================================================================= */

typedef struct _YJ2_TreeNode
{
	uint16 weight;
	uint16 value;
	struct _YJ2_TreeNode* parent;
	struct _YJ2_TreeNode* left;
	struct _YJ2_TreeNode* right;
} YJ2_TreeNode;

typedef struct _YJ2_Tree
{
	YJ2_TreeNode* node;
	YJ2_TreeNode** list;
} YJ2_Tree;

static uint8 yj2_data1[0x100] =
{
	0x3f, 0x0b, 0x17, 0x03, 0x2f, 0x0a, 0x16, 0x00, 0x2e, 0x09, 0x15, 0x02, 0x2d, 0x01, 0x08, 0x00,
	0x3e, 0x07, 0x14, 0x03, 0x2c, 0x06, 0x13, 0x00, 0x2b, 0x05, 0x12, 0x02, 0x2a, 0x01, 0x04, 0x00,
	0x3d, 0x0b, 0x11, 0x03, 0x29, 0x0a, 0x10, 0x00, 0x28, 0x09, 0x0f, 0x02, 0x27, 0x01, 0x08, 0x00,
	0x3c, 0x07, 0x0e, 0x03, 0x26, 0x06, 0x0d, 0x00, 0x25, 0x05, 0x0c, 0x02, 0x24, 0x01, 0x04, 0x00,
	0x3b, 0x0b, 0x17, 0x03, 0x23, 0x0a, 0x16, 0x00, 0x22, 0x09, 0x15, 0x02, 0x21, 0x01, 0x08, 0x00,
	0x3a, 0x07, 0x14, 0x03, 0x20, 0x06, 0x13, 0x00, 0x1f, 0x05, 0x12, 0x02, 0x1e, 0x01, 0x04, 0x00,
	0x39, 0x0b, 0x11, 0x03, 0x1d, 0x0a, 0x10, 0x00, 0x1c, 0x09, 0x0f, 0x02, 0x1b, 0x01, 0x08, 0x00,
	0x38, 0x07, 0x0e, 0x03, 0x1a, 0x06, 0x0d, 0x00, 0x19, 0x05, 0x0c, 0x02, 0x18, 0x01, 0x04, 0x00,
	0x37, 0x0b, 0x17, 0x03, 0x2f, 0x0a, 0x16, 0x00, 0x2e, 0x09, 0x15, 0x02, 0x2d, 0x01, 0x08, 0x00,
	0x36, 0x07, 0x14, 0x03, 0x2c, 0x06, 0x13, 0x00, 0x2b, 0x05, 0x12, 0x02, 0x2a, 0x01, 0x04, 0x00,
	0x35, 0x0b, 0x11, 0x03, 0x29, 0x0a, 0x10, 0x00, 0x28, 0x09, 0x0f, 0x02, 0x27, 0x01, 0x08, 0x00,
	0x34, 0x07, 0x0e, 0x03, 0x26, 0x06, 0x0d, 0x00, 0x25, 0x05, 0x0c, 0x02, 0x24, 0x01, 0x04, 0x00,
	0x33, 0x0b, 0x17, 0x03, 0x23, 0x0a, 0x16, 0x00, 0x22, 0x09, 0x15, 0x02, 0x21, 0x01, 0x08, 0x00,
	0x32, 0x07, 0x14, 0x03, 0x20, 0x06, 0x13, 0x00, 0x1f, 0x05, 0x12, 0x02, 0x1e, 0x01, 0x04, 0x00,
	0x31, 0x0b, 0x11, 0x03, 0x1d, 0x0a, 0x10, 0x00, 0x1c, 0x09, 0x0f, 0x02, 0x1b, 0x01, 0x08, 0x00,
	0x30, 0x07, 0x0e, 0x03, 0x1a, 0x06, 0x0d, 0x00, 0x19, 0x05, 0x0c, 0x02, 0x18, 0x01, 0x04, 0x00
};
static uint8 yj2_data2[0x10] =
{
	0x08, 0x05, 0x06, 0x04, 0x07, 0x05, 0x06, 0x03, 0x07, 0x05, 0x06, 0x04, 0x07, 0x04, 0x05, 0x03
};

static void yj2_adjust_tree(YJ2_Tree tree, uint16 value)
{
	YJ2_TreeNode* node = tree.list[value];
	YJ2_TreeNode tmp;
	YJ2_TreeNode* tmp1;
	YJ2_TreeNode* temp;
	while (node->value != 0x280)
	{
		temp = node + 1;
		while (node->weight == temp->weight)
			temp++;
		temp--;
		if (temp != node)
		{
			tmp1 = node->parent;
			node->parent = temp->parent;
			temp->parent = tmp1;
			if (node->value > 0x140)
			{
				node->left->parent = temp;
				node->right->parent = temp;
			}
			else
				tree.list[node->value] = temp;
			if (temp->value > 0x140)
			{
				temp->left->parent = node;
				temp->right->parent = node;
			}
			else
				tree.list[temp->value] = node;
			tmp = *node; *node = *temp; *temp = tmp;
			node = temp;
		}
		node->weight++;
		node = node->parent;
	}
	node->weight++;
}

static int32 yj2_build_tree(YJ2_Tree* tree)
{
	int32 i, ptr;
	YJ2_TreeNode** list;
	YJ2_TreeNode* node;
	if ((tree->list = list = (YJ2_TreeNode**)FMemory::Malloc(sizeof(YJ2_TreeNode*) * 321)) == nullptr)
		return 0;
	if ((tree->node = node = (YJ2_TreeNode*)FMemory::Malloc(sizeof(YJ2_TreeNode) * 641)) == nullptr)
	{
		FMemory::Free(list);
		return 0;
	}
	FMemory::Memset(list, 0, 321 * sizeof(YJ2_TreeNode*));
	FMemory::Memset(node, 0, 641 * sizeof(YJ2_TreeNode));
	for (i = 0; i <= 0x140; i++)
		list[i] = node + i;
	for (i = 0; i <= 0x280; i++)
	{
		node[i].value = i;
		node[i].weight = 1;
	}
	tree->node[0x280].parent = tree->node + 0x280;
	for (i = 0, ptr = 0x141; ptr <= 0x280; i += 2, ptr++)
	{
		node[ptr].left = node + i;
		node[ptr].right = node + i + 1;
		node[i].parent = node[i + 1].parent = node + ptr;
		node[ptr].weight = node[i].weight + node[i + 1].weight;
	}
	return 1;
}

static int32 yj2_bt(const uint8* data, uint32 pos)
{
	return (data[pos >> 3] & (uint8)(1 << (pos & 0x7))) >> (pos & 0x7);
}


SIZE_T
YJ2_Decompress(
	uint8* Source,
	uint8* Destination,
	SIZE_T DestSize
)
{
	int32 Length;
	uint32 len = 0, ptr = 0;
	uint8* src = (uint8*)Source + 4;
	uint8* dest;
	YJ2_Tree tree;
	YJ2_TreeNode* node;

	if (Source == nullptr)
		return -1;

	if (!yj2_build_tree(&tree))
		return -1;

	Length = *((uint32*)Source);
	if (Length > DestSize)
		return -1;
	dest = (uint8*)Destination;

	while (1)
	{
		uint16 val;
		node = tree.node + 0x280;
		while (node->value > 0x140)
		{
			if (yj2_bt(src, ptr))
				node = node->right;
			else
				node = node->left;
			ptr++;
		}
		val = node->value;
		if (tree.node[0x280].weight == 0x8000)
		{
			int32 i;
			for (i = 0; i < 0x141; i++)
				if (tree.list[i]->weight & 0x1)
					yj2_adjust_tree(tree, i);
			for (i = 0; i <= 0x280; i++)
				tree.node[i].weight >>= 1;
		}
		yj2_adjust_tree(tree, val);
		if (val > 0xff)
		{
			int32 i;
			uint32 temp, tmp, pos;
			uint8* pre;
			for (i = 0, temp = 0; i < 8; i++, ptr++)
				temp |= (uint32)yj2_bt(src, ptr) << i;
			tmp = temp & 0xff;
			for (; i < yj2_data2[tmp & 0xf] + 6; i++, ptr++)
				temp |= (uint32)yj2_bt(src, ptr) << i;
			temp >>= yj2_data2[tmp & 0xf];
			pos = (temp & 0x3f) | ((uint32)yj2_data1[tmp] << 6);
			if (pos == 0xfff)
				break;
			pre = dest - pos - 1;
			for (i = 0; i < val - 0xfd; i++)
				*dest++ = *pre++;
			len += val - 0xfd;
		}
		else
		{
			*dest++ = (uint8)val;
			len++;
		}
	}

	FMemory::Free(tree.list);
	FMemory::Free(tree.node);
	return Length;
}
