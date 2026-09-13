#ifndef ID3V240_SPEC_FRAMES_H
# define ID3V240_SPEC_FRAMES_H

struct fr_wrapper {
	struct frame	*frame;

	FILE	*(*stream)(FILE *, struct fr_wrapper *);
	FILE	*(*stream_full)(FILE *, struct fr_wrapper *);
}

fr_wrapper	*fr_wrapper_init(struct fr_wrapper *fw);

fr_wrapper	*fr_wrapper_new();

fr_wrapper	*fr_wrapper_cpy(struct fr_wrapper *dest, struct fr_wrapper *src);

struct aenc {
	//	<Header for 'Audio encryption', ID: "AENC">
	//	Owner identifier   <text string> $00
	//	Preview start      $xx xx
	//	Preview length     $xx xx
	//	Encryption info    <binary data>
	char	*owner;
	u32		start;
	u32		length;
	char	*info;
	u32		*info_len;

	struct fr_wrapper	*(*cpy)(struct fr_wrapper *dest, struct fr_wrapper *src);
	void							(del)(struct fr_wrapper *fw);
};



#endif
