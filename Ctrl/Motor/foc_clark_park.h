
#ifndef _FOC_CLARK_PARK_H_
#define _FOC_CLARK_PARK_H_


typedef struct{
    float a;
    float b;
    float c;
}abc_t;

typedef struct{
    float q;
    float d;
}qd_t;

typedef struct
{
  float alpha;
  float beta;
}alphabeta_t;



void Clark_Park_Transform(void);
alphabeta_t Inverse_Park_transform(qd_t Iqd, float theta);
qd_t Park_transform(alphabeta_t Ialphabeta, float theta);
abc_t Inverse_Clark_transform(alphabeta_t Ialphabeta);
alphabeta_t Clark_transform(abc_t Iabc);
#endif /* _FOC_CLARK_PARK_H_ */