static unsigned char jpg_def[] = {
	0xFF, 0xD8, 0xFF, 0xE0, 0x00, 0x10, 0x4A, 0x46, 0x49, 0x46, 0x00, 0x01, 0x01, 0x01, 0x00, 0x60,
	0x00, 0x60, 0x00, 0x00, 0xFF, 0xDB, 0x00, 0x43, 0x00, 0x08, 0x06, 0x06, 0x07, 0x06, 0x05, 0x08,
	0x07, 0x07, 0x07, 0x09, 0x09, 0x08, 0x0A, 0x0C, 0x14, 0x0D, 0x0C, 0x0B, 0x0B, 0x0C, 0x19, 0x12,
	0x13, 0x0F, 0x14, 0x1D, 0x1A, 0x1F, 0x1E, 0x1D, 0x1A, 0x1C, 0x1C, 0x20, 0x24, 0x2E, 0x27, 0x20,
	0x22, 0x2C, 0x23, 0x1C, 0x1C, 0x28, 0x37, 0x29, 0x2C, 0x30, 0x31, 0x34, 0x34, 0x34, 0x1F, 0x27,
	0x39, 0x3D, 0x38, 0x32, 0x3C, 0x2E, 0x33, 0x34, 0x32, 0xFF, 0xDB, 0x00, 0x43, 0x01, 0x09, 0x09,
	0x09, 0x0C, 0x0B, 0x0C, 0x18, 0x0D, 0x0D, 0x18, 0x32, 0x21, 0x1C, 0x21, 0x32, 0x32, 0x32, 0x32,
	0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32,
	0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32,
	0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0xFF, 0xC0,
	0x00, 0x11, 0x08, 0x00, 0x41, 0x00, 0x2F, 0x03, 0x01, 0x22, 0x00, 0x02, 0x11, 0x01, 0x03, 0x11,
	0x01, 0xFF, 0xC4, 0x00, 0x1F, 0x00, 0x00, 0x01, 0x05, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
	0x0A, 0x0B, 0xFF, 0xC4, 0x00, 0xB5, 0x10, 0x00, 0x02, 0x01, 0x03, 0x03, 0x02, 0x04, 0x03, 0x05,
	0x05, 0x04, 0x04, 0x00, 0x00, 0x01, 0x7D, 0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12, 0x21,
	0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07, 0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xA1, 0x08, 0x23,
	0x42, 0xB1, 0xC1, 0x15, 0x52, 0xD1, 0xF0, 0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0A, 0x16, 0x17,
	0x18, 0x19, 0x1A, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A,
	0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A,
	0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A,
	0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99,
	0x9A, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7,
	0xB8, 0xB9, 0xBA, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xD2, 0xD3, 0xD4, 0xD5,
	0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xF1,
	0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFF, 0xC4, 0x00, 0x1F, 0x01, 0x00, 0x03,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
	0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0xFF, 0xC4, 0x00, 0xB5, 0x11, 0x00,
	0x02, 0x01, 0x02, 0x04, 0x04, 0x03, 0x04, 0x07, 0x05, 0x04, 0x04, 0x00, 0x01, 0x02, 0x77, 0x00,
	0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21, 0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71, 0x13,
	0x22, 0x32, 0x81, 0x08, 0x14, 0x42, 0x91, 0xA1, 0xB1, 0xC1, 0x09, 0x23, 0x33, 0x52, 0xF0, 0x15,
	0x62, 0x72, 0xD1, 0x0A, 0x16, 0x24, 0x34, 0xE1, 0x25, 0xF1, 0x17, 0x18, 0x19, 0x1A, 0x26, 0x27,
	0x28, 0x29, 0x2A, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
	0x4A, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
	0x6A, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88,
	0x89, 0x8A, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6,
	0xA7, 0xA8, 0xA9, 0xAA, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xC2, 0xC3, 0xC4,
	0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xE2,
	0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9,
	0xFA, 0xFF, 0xDA, 0x00, 0x0C, 0x03, 0x01, 0x00, 0x02, 0x11, 0x03, 0x11, 0x00, 0x3F, 0x00, 0xF7,
	0xFA, 0x8E, 0x79, 0xE1, 0xB5, 0xB7, 0x96, 0xE2, 0xE2, 0x58, 0xE1, 0x82, 0x24, 0x2F, 0x24, 0x92,
	0x30, 0x55, 0x45, 0x03, 0x24, 0x92, 0x78, 0x00, 0x0E, 0x73, 0x52, 0x54, 0x73, 0xC1, 0x0D, 0xD5,
	0xBC, 0xB6, 0xF7, 0x11, 0x47, 0x34, 0x12, 0xA1, 0x49, 0x23, 0x91, 0x43, 0x2B, 0xA9, 0x18, 0x20,
	0x83, 0xC1, 0x04, 0x71, 0x8A, 0x00, 0xE1, 0xE0, 0xF8, 0xCF, 0xF0, 0xFA, 0xE6, 0xE2, 0x28, 0x13,
	0xC4, 0x51, 0x87, 0x91, 0xC2, 0x29, 0x92, 0xDA, 0x64, 0x50, 0x49, 0xC7, 0x2C, 0xC8, 0x02, 0x8F,
	0x72, 0x40, 0x1D, 0xEB, 0xB8, 0x82, 0x78, 0x6E, 0xAD, 0xE2, 0xB8, 0xB7, 0x96, 0x39, 0xA0, 0x95,
	0x03, 0xC7, 0x24, 0x6C, 0x19, 0x5D, 0x48, 0xC8, 0x20, 0x8E, 0x08, 0x23, 0x9C, 0xD7, 0x9B, 0xEA,
	0xBF, 0x02, 0x3C, 0x0F, 0x7B, 0xA5, 0xDC, 0x5B, 0xD8, 0x69, 0xD2, 0x69, 0xF7, 0x6E, 0x98, 0x8A,
	0xE9, 0x2E, 0x25, 0x90, 0xC4, 0xDD, 0x8E, 0xD7, 0x72, 0x18, 0x76, 0x23, 0xD0, 0x9C, 0x10, 0x70,
	0x47, 0x94, 0x69, 0x1E, 0x21, 0xD5, 0xFE, 0x05, 0xF8, 0xF2, 0xEF, 0x41, 0xBC, 0xB9, 0x8F, 0x52,
	0xD1, 0xDD, 0xD5, 0xE7, 0x86, 0x07, 0x04, 0x95, 0x61, 0xF2, 0xCA, 0xAB, 0x9F, 0xDD, 0xCB, 0xB7,
	0x19, 0x43, 0xD4, 0x63, 0x92, 0x36, 0x38, 0x00, 0xFA, 0x8E, 0x8A, 0xAF, 0x61, 0x7D, 0x6F, 0xA9,
	0xE9, 0xD6, 0xD7, 0xF6, 0x72, 0x79, 0x96, 0xB7, 0x51, 0x24, 0xD0, 0xBE, 0xD2, 0x37, 0x23, 0x00,
	0x54, 0xE0, 0xF2, 0x32, 0x08, 0xEB, 0x56, 0x28, 0x00, 0xA2, 0x8A, 0x28, 0x03, 0xE6, 0xCF, 0x88,
	0xFF, 0x00, 0x07, 0x3C, 0x61, 0xA8, 0x78, 0xD6, 0xFB, 0x53, 0xD2, 0xA3, 0x93, 0x58, 0x82, 0xF9,
	0xDA, 0xE0, 0xCA, 0xF3, 0x47, 0x11, 0x84, 0x96, 0x6C, 0x43, 0xF3, 0xC9, 0x92, 0x15, 0x42, 0x00,
	0x78, 0x18, 0xC0, 0x00, 0x62, 0xBC, 0xDE, 0x4F, 0x0D, 0xEA, 0x5E, 0x14, 0xD5, 0x21, 0xB8, 0xF1,
	0x57, 0x86, 0x35, 0x23, 0xA7, 0xC3, 0x71, 0xE5, 0xCB, 0x1B, 0x6E, 0x81, 0x27, 0x61, 0x9F, 0x91,
	0x66, 0xDA, 0x41, 0x07, 0x69, 0x39, 0x5C, 0xE4, 0x03, 0x82, 0x3A, 0x8F, 0x73, 0xF1, 0x57, 0xC7,
	0xEF, 0xF8, 0x46, 0xBC, 0x51, 0xA8, 0xE8, 0xBF, 0xF0, 0x8A, 0xCF, 0x2F, 0xD8, 0xE5, 0x31, 0x79,
	0x93, 0xDD, 0xF9, 0x2C, 0xF8, 0x1F, 0x78, 0x2E, 0xC6, 0xF9, 0x4F, 0x55, 0x39, 0xE5, 0x48, 0x3C,
	0x67, 0x15, 0x9F, 0x0F, 0xED, 0x0B, 0xE1, 0xBD, 0x62, 0xCE, 0xE6, 0xCF, 0xC4, 0xBE, 0x14, 0x9C,
	0xDA, 0xBE, 0xDC, 0x42, 0x86, 0x3B, 0xB4, 0x93, 0x07, 0x3F, 0x32, 0xB8, 0x40, 0x30, 0x42, 0x91,
	0xD7, 0xF0, 0xC7, 0x20, 0x1E, 0x99, 0xF0, 0xFB, 0xC5, 0x7E, 0x1C, 0xF1, 0x3F, 0x86, 0xA1, 0x5F,
	0x0E, 0x19, 0x22, 0xB7, 0xB0, 0x45, 0xB6, 0x36, 0x73, 0xFF, 0x00, 0xAD, 0xB7, 0x55, 0x18, 0x40,
	0xC3, 0x27, 0x20, 0xAA, 0xF0, 0xD9, 0x39, 0xC1, 0xE7, 0x20, 0x81, 0xD6, 0x57, 0xCD, 0x9F, 0x01,
	0xE4, 0xB1, 0x9B, 0xE2, 0xE6, 0xBD, 0x2E, 0x97, 0x0C, 0x90, 0x69, 0xEF, 0x65, 0x70, 0xD6, 0xB1,
	0x48, 0x72, 0xC9, 0x11, 0x9E, 0x2D, 0x8A, 0x79, 0x3C, 0x85, 0xC0, 0xEA, 0x7E, 0xA6, 0xBE, 0x93,
	0xA0, 0x02, 0x8A, 0x28, 0xA0, 0x0A, 0xF0, 0xDF, 0xD9, 0xDC, 0x5E, 0x5C, 0xD9, 0xC3, 0x77, 0x04,
	0x97, 0x56, 0xBB, 0x7E, 0xD1, 0x0A, 0x48, 0x0B, 0xC5, 0xB8, 0x65, 0x77, 0x28, 0xE5, 0x72, 0x39,
	0x19, 0xEB, 0x59, 0xF7, 0xDE, 0x13, 0xF0, 0xDE, 0xA7, 0x79, 0x25, 0xE5, 0xFF, 0x00, 0x87, 0xF4,
	0xAB, 0xBB, 0xA9, 0x31, 0xBE, 0x69, 0xEC, 0xA3, 0x91, 0xDB, 0x00, 0x01, 0x96, 0x23, 0x27, 0x00,
	0x01, 0xF8, 0x57, 0xCC, 0x03, 0xC1, 0x17, 0x9E, 0x3D, 0xF8, 0xBF, 0xE2, 0xCD, 0x2A, 0xCA, 0xEA,
	0x0B, 0x69, 0x22, 0xBB, 0xBE, 0xB9, 0x2F, 0x30, 0x24, 0x1D, 0xB3, 0x15, 0x55, 0xE0, 0x77, 0x66,
	0x50, 0x4F, 0x61, 0x93, 0xC9, 0x18, 0x3D, 0xA4, 0x1F, 0x09, 0x3E, 0x2C, 0xDA, 0xDB, 0xC5, 0x6F,
	0x6F, 0xE3, 0x78, 0xE1, 0x82, 0x24, 0x09, 0x1C, 0x71, 0xEA, 0xD7, 0x4A, 0xA8, 0xA0, 0x60, 0x00,
	0x02, 0x60, 0x00, 0x38, 0xC5, 0x00, 0x7B, 0xBE, 0x9B, 0xA4, 0xE9, 0xBA, 0x35, 0xBB, 0x5B, 0xE9,
	0x7A, 0x7D, 0xA5, 0x8C, 0x0C, 0xE5, 0xDA, 0x3B, 0x58, 0x56, 0x25, 0x2D, 0x80, 0x32, 0x42, 0x80,
	0x33, 0x80, 0x06, 0x7D, 0x85, 0x5C, 0xAF, 0x9F, 0xFF, 0x00, 0xE1, 0x56, 0x7C, 0x5F, 0xFF, 0x00,
	0xA1, 0xF7, 0xFF, 0x00, 0x2B, 0x17, 0x7F, 0xFC, 0x45, 0x7A, 0xC7, 0x80, 0x34, 0x5F, 0x10, 0x68,
	0x1E, 0x17, 0x4B, 0x1F, 0x12, 0xEB, 0x3F, 0xDA, 0xB7, 0xE2, 0x57, 0x71, 0x36, 0xE6, 0x7D, 0x88,
	0x4F, 0x09, 0xBD, 0x80, 0x67, 0xEE, 0x72, 0x47, 0x1B, 0xB6, 0xF4, 0x51, 0x40, 0x1D, 0x45, 0x14,
	0x57, 0x27, 0xE3, 0x8F, 0x88, 0x7A, 0x2F, 0x80, 0x2D, 0xEC, 0xE5, 0xD5, 0x92, 0xEE, 0x57, 0xBC,
	0x76, 0x58, 0x62, 0xB5, 0x8C, 0x33, 0x10, 0xA0, 0x6E, 0x63, 0xB8, 0x80, 0x00, 0xDC, 0xA3, 0xAE,
	0x7E, 0x61, 0x80, 0x79, 0xC0, 0x07, 0x9D, 0xF8, 0xDB, 0xE0, 0xAE, 0xB1, 0x27, 0x88, 0xF5, 0x0F,
	0x15, 0x78, 0x3F, 0x58, 0xFB, 0x2D, 0xFC, 0xD2, 0xB5, 0xC2, 0xDA, 0xAB, 0x3C, 0x2E, 0xAC, 0xC8,
	0x7C, 0xCD, 0x93, 0x6E, 0x3F, 0x33, 0xB1, 0x3C, 0x10, 0xAB, 0xF3, 0x91, 0x90, 0x05, 0x73, 0x96,
	0x7F, 0x16, 0xFE, 0x21, 0xF8, 0x05, 0xE0, 0xD3, 0xFC, 0x5B, 0xA3, 0x49, 0x77, 0x02, 0x3B, 0xC6,
	0xAF, 0x7B, 0x1B, 0x45, 0x2C, 0xA1, 0x14, 0x2E, 0x12, 0x61, 0x95, 0x70, 0x0E, 0x09, 0x6C, 0x39,
	0x3B, 0xBE, 0xF7, 0x20, 0x8E, 0xCF, 0xFE, 0x1A, 0x3B, 0xC1, 0xFF, 0x00, 0xF4, 0x0D, 0xD7, 0x3F,
	0xEF, 0xC4, 0x3F, 0xFC, 0x76, 0xB3, 0xF5, 0xCF, 0x8F, 0x7E, 0x07, 0xD6, 0xB4, 0x3B, 0xDD, 0x3A,
	0xEB, 0x40, 0xD5, 0x6F, 0x61, 0x9E, 0x26, 0x53, 0x6F, 0x3C, 0x71, 0x22, 0x48, 0x7A, 0xA8, 0x2C,
	0x24, 0x25, 0x79, 0x03, 0xE6, 0x00, 0x95, 0xEA, 0x39, 0x14, 0x01, 0xEB, 0x1E, 0x18, 0xF1, 0x3E,
	0x97, 0xE2, 0xED, 0x0E, 0x1D, 0x5F, 0x48, 0x9F, 0xCD, 0xB7, 0x93, 0x86, 0x56, 0xE1, 0xE2, 0x71,
	0xD5, 0x1C, 0x76, 0x61, 0x91, 0xF9, 0x82, 0x09, 0x04, 0x13, 0xB1, 0x5E, 0x17, 0xFB, 0x36, 0x69,
	0x57, 0xD6, 0xBA, 0x5E, 0xBB, 0xA9, 0xCF, 0x6D, 0x24, 0x76, 0x77, 0xAF, 0x02, 0x5B, 0x4A, 0xDC,
	0x09, 0x4C, 0x7E, 0x66, 0xFD, 0xBD, 0xC8, 0x05, 0x80, 0xCF, 0x4C, 0xE4, 0x75, 0x07, 0x1E, 0xE9,
	0x40, 0x05, 0x78, 0xFF, 0x00, 0xED, 0x1D, 0xFF, 0x00, 0x24, 0xF3, 0x4F, 0xFF, 0x00, 0xB0, 0xAC,
	0x7F, 0xFA, 0x2A, 0x5A, 0x28, 0xA0, 0x0F, 0x98, 0x28, 0xA2, 0x8A, 0x00, 0xFB, 0xFE, 0x8A, 0x28,
	0xA0, 0x0F, 0xFF, 0xD9, 
};
