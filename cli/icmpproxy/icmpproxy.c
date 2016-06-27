/* icmpproxy - tunnel traffic over icmp
 * Copyright (C) 2004 Vidar 'koala_man' Holen
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version. 
 *
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details. 
 *
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 675 Mass
 * Ave, Cambridge, MA 02139, USA. 
 * 
 */

#include "icmpproxy.h"

/* Ripped from linux-2.6.4/Documentation/networking/tuntap.txt */
int opentun(char* dev) {
    struct ifreq ifr; 
    int fd, err;
    
    if((fd=open("/dev/misc/net/tun",O_RDWR))<0) {
        if((fd=open("/dev/net/tun",O_RDWR))<0) {
            perror("Opening tun device");
            exit(1);
        }
    }

    memset(&ifr,0,sizeof(ifr));

    ifr.ifr_flags=IFF_TUN | IFF_NO_PI;
    strncpy(ifr.ifr_name,dev,IFNAMSIZ);
    
    if((err=ioctl(fd, TUNSETIFF, (void*)&ifr)) < 0) {
        perror("TUNSETIFF");
        close(fd);
        exit(2);
    }
    
    strcpy(dev,ifr.ifr_name);
    return fd;
}

/* Ripped from iputils/ping.c */
unsigned short in_sum(char* packet, int len, unsigned short csum) {
    int nleft=len;
    const unsigned short* data=(short*)packet;
    int sum=csum;

    while(nleft>1) {
        sum+=*data++;
        nleft-=2;
    }
    if(nleft == 1) 
        sum+=htons(packet[len-1] << 8);
    sum=(sum>>16) + (sum & 0xffff);
    sum+=(sum>>16);
    
    return (unsigned short)(~sum);
}
char* ntoa(int n) {
    struct in_addr lol;
    lol.s_addr=htonl(n);
    return (char*)inet_ntoa(lol);
}
int findtarget(int addr) {
    int lol=targets[addr];
    dbg("findtarget: x.x.x.%d -> %s\n",addr,ntoa(ntohl(lol)));
    return lol;
}

int settarget(int addr, int target) {
    int r=findtarget(addr);
    targets[addr]=target;
    dbg("settarget: Packets for %d go to %s\n",addr,ntoa(ntohl(target)));
}


int main(int argc, char** argv) {
    char devname[IFNAMSIZ];
    int ifd, pingd;
    fd_set fds;
    int maxfd;
    struct timeval tv;

    struct in_addr me;
    int mask, server;
    
    char packet[PACKLEN];
    int len,tmp;
    struct sockaddr_in sa;
    struct in_addr buf;
    socklen_t buflen;
    struct icmphdr* icmph;

    int iphl;
    int faddr, taddr;

    int id, seq=0;

    if(argc<3) {
        dbg("ICMP-Proxy, by koala_man\n");
        dbg("Usage: %s magic ip\n",argv[0]);
        dbg("Magic is any 2-char string, same for server and client\n");
        dbg("For clients: ip is the public address for the server\n");
        dbg("For servers: ip is the private /24-mask for clients\n");
        exit(1);
    }
    if(inet_aton(argv[2],&me)<0) {
        perror("inet_aton");
        exit(1);
    }
    if(argv[1][0]!=0) id=argv[1][1]<<8;
    id=id | argv[1][0];
    dbg("Using id %x\n",id);

    server=(((me.s_addr)>>24)&0xFF)==0;
    mask=htonl(me.s_addr);
    dbg("You're a %s\n",server?"server":"client");
    dbg("Mask is %d: %s\n",mask,ntoa(mask));

    strncpy(devname,"it%d",IFNAMSIZ);
    ifd=opentun(devname);
    dbg("Opened tun as %s\n",devname);

    if((pingd=socket(AF_INET,SOCK_RAW,IPPROTO_ICMP))<0) {
        perror("socket");
        close(ifd);
        exit(3);
    }

    targets=(int*)calloc(256,sizeof(struct in_addr));
    
    maxfd=MAX(ifd,pingd)+1;
    FD_ZERO(&fds);
    
    while(1) {
        FD_SET(pingd,&fds);
        FD_SET(ifd,&fds);
        tv.tv_sec=8; //XXX
        tv.tv_usec=0;
        
        dbg("\n");
        tmp=select(maxfd,&fds,NULL,NULL,&tv);
        if(tmp==0) {
            if(server) continue;
            icmph=(struct icmphdr*) packet;
            icmph->type=ICMP_ECHO;
            icmph->code=0;
            icmph->checksum=0;
            icmph->un.echo.sequence=seq++;
            icmph->un.echo.id=id;
            tmp=(sizeof(struct icmphdr));
            icmph->checksum=in_sum(packet,tmp,0);
            buflen=sizeof(struct sockaddr);
            sa.sin_addr=me;
            sendto(pingd,packet,tmp,0,(struct sockaddr*)&sa,buflen);
            dbg("Ping!\n");
        } else {
            if(FD_ISSET(pingd,&fds)) {
                dbg("Got a ping\n");
                buflen=sizeof(struct sockaddr);
                len=recvfrom(pingd,packet,PACKLEN,0,(struct sockaddr*)&sa,&buflen);
                dbg("Recvfrom=%d\n",len);
                iphl=packet[0]&0x0F;
                dbg("Packet header: %d\n",iphl);
                if(iphl<5) { 
                    dbg("Bad packet, too short header.\n");
                    continue;
                }
                taddr=ntohl(((int*)packet)[3]); //tunnel from
                faddr=ntohl(((int*)packet)[iphl+8/4+3]); //packet from
                if(!(taddr==mask || (server && (faddr & ~0xff)==mask))) {
                    dbg("IP from wrong place\n");
                    dbg(" Is %s\n",ntoa(faddr));
                    dbg(" or %s\n",ntoa(taddr));
                    dbg(" Should be %s\n",ntoa(mask));
                    continue;
                } 
                icmph=(struct icmphdr*)(packet+iphl*4);
                if(icmph->type != ICMP_ECHOREPLY) {
                    dbg("Not ICMP_ECHOREPLY\n");
                    continue;
                }
                if(icmph->un.echo.id!=id) {
                    dbg("Wrong ID\n");
                    continue;
                }
                if((len-iphl*4)<20 || len>PACKLEN) {
                    dbg("Not sane size\n");
                    continue;
                }
                if(server) {
                    dbg("Setting %x to %x\n",faddr,taddr);
                    settarget(faddr & 0xff,htonl(taddr));
                }
                write(ifd,packet+iphl*4+8,len-iphl*4-8);
            } 
            if(FD_ISSET(ifd,&fds)) {
                dbg("Got from if\n");
                icmph=(struct icmphdr*) packet;
                icmph->type=ICMP_ECHOREPLY;
                icmph->code=0;
                icmph->checksum=0;
                icmph->un.echo.sequence=htons(seq++);
                icmph->un.echo.id=id;
                
                len=read(ifd,packet+8,TUNLEN);  
                dbg("read=%d\n",len);
                if(len<0) {
                    perror("Read from if");
                    exit(4);
                }
                if(len==0) {
                    dbg("no data, lol?\n");
                    continue;
                }
                icmph->checksum=in_sum(packet,8+len,0);

                buflen=sizeof(struct sockaddr);
                if(server) {
                    tmp=ntohl(((int*)packet)[8/4+4]);
                    buf.s_addr=findtarget(tmp&0xff);
                    if(buf.s_addr==0) {
                        dbg("Don't know where to send, dropping\n");
                        continue;
                    }
                    sa.sin_addr=buf;
                } else {
                    sa.sin_addr=me;
                }
                sendto(pingd,packet,len+8,0,(struct sockaddr*)&sa,buflen);
            }
        }
    }
 
    buflen=sizeof(struct sockaddr);
    len=recvfrom(pingd,packet,PACKLEN,0,(struct sockaddr*)&sa,&buflen);
    dbg("Recvfrom=%d\n",len);
    write(2,packet,len);

    close(ifd);
    close(pingd);
    return 0;
    }

    
