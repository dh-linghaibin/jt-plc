/* ����ͷ�ļ� *****************************************************************/
#include "ringbuf.h"
/* ˽���������� ***************************************************************/
/* ˽�г����� *****************************************************************/
/* ˽�ж����� *****************************************************************/
/* ˽�б��� *******************************************************************/
/* ȫ�ֱ��� */
/* ˽�к������� ***************************************************************/
/* ˽�к���  ******************************************************************/
int RingbufInit(Stdringbuf_n* r, uint8_t array[], uint16_t len) {
    if (len < 2 || array==NULL){
        return false;
    }
    r->buf = array;
    r->length = len;
    r->fill_cnt = 0;
    r->head = r->tail = 0;
    return true;
}

int RingbufPut(Stdringbuf_n* r, uint8_t data) {
    //��tail+1����headʱ��˵����������
    if (r->fill_cnt >= r->length) {
        //printf("BUF FULL!\n");
        return false;                  // ������������ˣ��򷵻ش���
    }
    r->buf[r->tail] = data;
    r->tail++;
    r->fill_cnt++;
    //����tail�Ƿ񳬳����鷶Χ������������Զ��л���0
    r->tail = r->tail % r->length;
    return true;
}

int RingbufGut(Stdringbuf_n* r, uint8_t *c, uint16_t length) {
    //��tail����headʱ��˵�����п�
    if (r->fill_cnt<=0) {
        //printf("BUF EMPTY!\n");
        return false;
    }
    //���ֻ�ܶ�ȡr->length��������
    if (length > r->length) {
        length = r->length;
    }
    
    if(r->tail > r->head) {
        if(r->tail < (length + r->head)) {
            return false;
        }
    } else {
        if( ((r->length-r->head)+r->tail) < length ) {
            return false;
        }
    }
    
    int i;
    for (i = 0; i<length; i++)
    {
        r->fill_cnt--;
        *c = r->buf[r->head++];                 // �������ݸ�*c
        *c++;
        //����head�ԼӺ���±��Ƿ񳬳����鷶Χ������������Զ��л���0
        r->head = r->head % r->length;
    }
    return true;
}

uint16_t RingbufNum(Stdringbuf_n* r) {
    if(r->tail == r->head) {
        return 0x00;
    } else {
        if(r->tail > r->head) {
            return (r->tail - r->head);
        } else {
            return (400-r->head) + r->tail;
        }
    }
} 

/***************************************************************END OF FILE****/
