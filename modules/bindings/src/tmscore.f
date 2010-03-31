*************************************************************************
*     This program is to compare two protein structures and identify the 
*     best superposition that has the maximum TM-score. The program can 
*     be freely copied or modified.
*     For comments, please email to: yzhang@ku.edu
*
*     Reference: 
*     Yang Zhang, Jeffrey Skolnick, Proteins, 2004 57:702-10.
*
******************* Updating history ************************************
*     2005/10/19: the program was reformed so that the score values
*                 are not dependent on the specific compilers.
*     2006/06/20: select 'A' if there is altLoc when reading PDB file
*     2007/02/05: fix a bug with length<15 in TMscore_32
*     2007/02/27: rotation matrix from Chain-1 to Chain-2 is added
*     2007/12/06: GDT-HA score is added, fixed a bug for reading PDB
*************************************************************************
      
      program TMscore
      PARAMETER(nmax=3000)
      
      common/stru/xt(nmax),yt(nmax),zt(nmax),xb(nmax),yb(nmax),zb(nmax)
      common/nres/nresA(nmax),nresB(nmax),nseqA,nseqB
      common/para/d,d0,d0_fix
      common/align/n_ali,iA(nmax),iB(nmax)
      common/nscore/i_ali(nmax),n_cut ![1,n_ali],align residues for the score
      dimension k_ali(nmax),k_ali0(nmax)

      character*100 fnam,pdb(100),outname
      character*3 aa(-1:20),seqA(nmax),seqB(nmax)
      character*100 s,du
      character seq1A(nmax),seq1B(nmax),ali(nmax)
      character sequenceA(nmax),sequenceB(nmax),sequenceM(nmax)

      dimension L_ini(100),iq(nmax)
      common/scores/score,score_maxsub,score_fix,score10
      common/GDT/n_GDT05,n_GDT1,n_GDT2,n_GDT4,n_GDT8
      double precision score,score_max,score_fix,score_fix_max
      double precision score_maxsub,score10
      dimension xa(nmax),ya(nmax),za(nmax)

ccc   RMSD:
      double precision r_1(3,nmax),r_2(3,nmax),r_3(3,nmax),w(nmax)
      double precision u(3,3),t(3),rms,drms !armsd is real
      data w /nmax*1.0/
ccc   

      data aa/ 'BCK','GLY','ALA','SER','CYS',
     &     'VAL','THR','ILE','PRO','MET',
     &     'ASP','ASN','LEU','LYS','GLU',
     &     'GLN','ARG','HIS','PHE','TYR',
     &     'TRP','CYX'/
      character*1 slc(-1:20)
      data slc/'X','G','A','S','C',
     &     'V','T','I','P','M',
     &     'D','N','L','K','E',
     &     'Q','R','H','F','Y',
     &     'W','C'/

*****instructions ----------------->
      call getarg(1,fnam)
      if(fnam.eq.' '.or.fnam.eq.'?'.or.fnam.eq.'-h')then
         write(*,*)
         write(*,*)'Brief instruction for running TM-score program:'
         write(*,*)
       write(*,*)'1. Run TM-score to compare ''model'' and ''native'':'
         write(*,*)'  TMscore model native'
         write(*,*)
      write(*,*)'2. Run TM-score with an assigned d0, e.g. 5 Angstroms:'
         write(*,*)'  TMscore model native -d 5'
         write(*,*)
         write(*,*)'3. Run TM-score with superposition output, e.g. ',
     &        '''TM.sup'':'
         write(*,*)'  TMscore model native -o TM.sup'
         write(*,*)
      write(*,*)'4, To view the superimposed structures by rasmol:'
         write(*,*)'   rasmol -script TM.sup'
         write(*,*)
         goto 9999
      endif
      
******* options ----------->
      m_out=-1
      m_fix=-1
      narg=iargc()
      i=0
      j=0
 115  continue
      i=i+1
      call getarg(i,fnam)
      if(fnam.eq.'-o')then
         m_out=1
         i=i+1
         call getarg(i,outname)
      elseif(fnam.eq.'-d')then
         m_fix=1
         i=i+1
         call getarg(i,fnam)
         read(fnam,*)d0_fix
      else
         j=j+1
         pdb(j)=fnam
      endif
      if(i.lt.narg)goto 115

ccccccccc read data from first CA file:
      open(unit=10,file=pdb(1),status='old')
      i=0
 101  read(10,104,end=102) s
      if(s(1:3).eq.'TER') goto 102
      if(s(1:4).eq.'ATOM')then
         if(s(13:16).eq.'CA  '.or.s(13:16).eq.' CA '.or.s(13:16).
     &        eq.'  CA')then
         if(s(17:17).eq.' '.or.s(17:17).eq.'A')then
            i=i+1
            read(s,103)du,seqA(i),du,nresA(i),du,xa(i),ya(i),za(i)
            do j=-1,20
               if(seqA(i).eq.aa(j))then
                  seq1A(i)=slc(j)
                  goto 21
               endif
            enddo
            seq1A(i)=slc(-1)
 21         continue
         endif
         endif
      endif
      goto 101
 102  continue
 103  format(A17,A3,A2,i4,A4,3F8.3)
 104  format(A100)
      close(10)
      nseqA=i
c^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
      
ccccccccc read data from first CA file:
      open(unit=10,file=pdb(2),status='old')
      i=0
 201  read(10,204,end=202) s
      if(s(1:3).eq.'TER') goto 202
      if(s(1:4).eq.'ATOM')then
         if(s(13:16).eq.'CA  '.or.s(13:16).eq.' CA '.or.s(13:16).
     &        eq.'  CA')then
         if(s(17:17).eq.' '.or.s(17:17).eq.'A')then
            i=i+1
            read(s,203)du,seqB(i),du,nresB(i),du,xb(i),yb(i),zb(i)
            do j=-1,20
               if(seqB(i).eq.aa(j))then
                  seq1B(i)=slc(j)
                  goto 22
               endif
            enddo
            seq1B(i)=slc(-1)
 22         continue
         endif
         endif
      endif
      goto 201
 202  continue
 203  format(A17,A3,A2,i4,A4,3F8.3)
 204  format(A100)
      close(10)
      nseqB=i
c^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

******************************************************************
*     pickup the aligned residues:
******************************************************************
      k=0
      do i=1,nseqA
         do j=1,nseqB
            if(nresA(i).eq.nresB(j))then
               k=k+1
               iA(k)=i
               iB(k)=j
               goto 205
            endif
         enddo
 205     continue
      enddo
      n_ali=k                   !number of aligned residues
      if(n_ali.lt.1)then
        write(*,*)'There is no common residues in the input structures'
        goto 9999
      endif
      
************/////
*     parameters:
*****************
***   d0------------->
      if(nseqB.gt.15)then
         d0=1.24*(nseqB-15)**(1.0/3.0)-1.8
      else
         d0=0.5
      endif
      if(d0.lt.0.5)d0=0.5
      if(m_fix.eq.1)d0=d0_fix
***   d0_search ----->
      d0_search=d0
      if(d0_search.gt.8)d0_search=8
      if(d0_search.lt.4.5)d0_search=4.5
***   iterative parameters ----->
      n_it=20                   !maximum number of iterations
      d_output=5                !for output alignment
      if(m_fix.eq.1)d_output=d0_fix
      n_init_max=6              !maximum number of L_init
      n_init=0
      L_ini_min=4
      if(n_ali.lt.4)L_ini_min=n_ali
      do i=1,n_init_max-1
         n_init=n_init+1
         L_ini(n_init)=n_ali/2**(n_init-1)
         if(L_ini(n_init).le.L_ini_min)then
            L_ini(n_init)=L_ini_min
            goto 402
         endif
      enddo
      n_init=n_init+1
      L_ini(n_init)=L_ini_min
 402  continue
      
******************************************************************
*     find the maximum score starting from local structures superposition
******************************************************************
      score_max=-1              !TM-score
      score_maxsub_max=-1       !MaxSub-score
      score10_max=-1            !TM-score10
      n_GDT05_max=-1            !number of residues<0.5
      n_GDT1_max=-1             !number of residues<1
      n_GDT2_max=-1             !number of residues<2
      n_GDT4_max=-1             !number of residues<4
      n_GDT8_max=-1             !number of residues<8
      do 333 i_init=1,n_init
        L_init=L_ini(i_init)
        iL_max=n_ali-L_init+1
        do 300 iL=1,iL_max      !on aligned residues, [1,nseqA]
           LL=0
           ka=0
           do i=1,L_init
              k=iL+i-1          ![1,n_ali] common aligned
              r_1(1,i)=xa(iA(k))
              r_1(2,i)=ya(iA(k))
              r_1(3,i)=za(iA(k))
              r_2(1,i)=xb(iB(k))
              r_2(2,i)=yb(iB(k))
              r_2(3,i)=zb(iB(k))
              ka=ka+1
              k_ali(ka)=k
              LL=LL+1
           enddo
           call u3b(w,r_1,r_2,LL,1,rms,u,t,ier) !u rotate r_1 to r_2
           if(i_init.eq.1)then  !global superposition
              armsd=dsqrt(rms/LL)
              rmsd_ali=armsd
           endif
           do j=1,nseqA
              xt(j)=t(1)+u(1,1)*xa(j)+u(1,2)*ya(j)+u(1,3)*za(j)
              yt(j)=t(2)+u(2,1)*xa(j)+u(2,2)*ya(j)+u(2,3)*za(j)
              zt(j)=t(3)+u(3,1)*xa(j)+u(3,2)*ya(j)+u(3,3)*za(j)
           enddo
           d=d0_search-1
           call score_fun       !init, get scores, n_cut+i_ali(i) for iteration
           if(score_max.lt.score)then
              score_max=score
              ka0=ka
              do i=1,ka0
                 k_ali0(i)=k_ali(i)
              enddo
           endif
           if(score10_max.lt.score10)score10_max=score10
           if(score_maxsub_max.lt.score_maxsub)score_maxsub_max=
     &          score_maxsub
           if(n_GDT05_max.lt.n_GDT05)n_GDT05_max=n_GDT05
           if(n_GDT1_max.lt.n_GDT1)n_GDT1_max=n_GDT1
           if(n_GDT2_max.lt.n_GDT2)n_GDT2_max=n_GDT2
           if(n_GDT4_max.lt.n_GDT4)n_GDT4_max=n_GDT4
           if(n_GDT8_max.lt.n_GDT8)n_GDT8_max=n_GDT8
***   iteration for extending ---------------------------------->
           d=d0_search+1
           do 301 it=1,n_it
              LL=0
              ka=0
              do i=1,n_cut
                 m=i_ali(i)     ![1,n_ali]
                 r_1(1,i)=xa(iA(m))
                 r_1(2,i)=ya(iA(m))
                 r_1(3,i)=za(iA(m))
                 r_2(1,i)=xb(iB(m))
                 r_2(2,i)=yb(iB(m))
                 r_2(3,i)=zb(iB(m))
                 ka=ka+1
                 k_ali(ka)=m
                 LL=LL+1
              enddo
              call u3b(w,r_1,r_2,LL,1,rms,u,t,ier) !u rotate r_1 to r_2
              do j=1,nseqA
                 xt(j)=t(1)+u(1,1)*xa(j)+u(1,2)*ya(j)+u(1,3)*za(j)
                 yt(j)=t(2)+u(2,1)*xa(j)+u(2,2)*ya(j)+u(2,3)*za(j)
                 zt(j)=t(3)+u(3,1)*xa(j)+u(3,2)*ya(j)+u(3,3)*za(j)
              enddo
              call score_fun    !get scores, n_cut+i_ali(i) for iteration
              if(score_max.lt.score)then
                 score_max=score
                 ka0=ka
                 do i=1,ka
                    k_ali0(i)=k_ali(i)
                 enddo
              endif
              if(score10_max.lt.score10)score10_max=score10
              if(score_maxsub_max.lt.score_maxsub)score_maxsub_max
     &             =score_maxsub
              if(n_GDT05_max.lt.n_GDT05)n_GDT05_max=n_GDT05
              if(n_GDT1_max.lt.n_GDT1)n_GDT1_max=n_GDT1
              if(n_GDT2_max.lt.n_GDT2)n_GDT2_max=n_GDT2
              if(n_GDT4_max.lt.n_GDT4)n_GDT4_max=n_GDT4
              if(n_GDT8_max.lt.n_GDT8)n_GDT8_max=n_GDT8
 303          format(i5,i5,i5,f17.14,f17.14,i5,f7.3)
              if(it.eq.n_it)goto 302
              if(n_cut.eq.ka)then
                 neq=0
                 do i=1,n_cut
                    if(i_ali(i).eq.k_ali(i))neq=neq+1
                 enddo
                 if(n_cut.eq.neq)goto 302
              endif
 301       continue             !for iteration
 302       continue
 300    continue                !for shift
 333  continue                  !for initial length, L_ali/M

******************************************************************
*     Output
******************************************************************
***   output TM-scale ---------------------------->
      write(*,*)
      write(*,*)'*****************************************************',
     &     '************************'
      write(*,*)'*                                 TM-SCORE           ',
     &     '                       *'
      write(*,*)'* A scoring function to assess the quality of protein',
     &     ' structure predictions *'
      write(*,*)'* Based on statistics:                               ',
     &     '                       *'
      write(*,*)'*       0.0 < TM-score < 0.17, Random predictions    ',
     &     '                       *'
      write(*,*)'*       0.4 < TM-score < 1.00, Meaningful predictions',
     &     '                       *'
      write(*,*)'* Reference: Yang Zhang and Jeffrey Skolnick, ',
     &     'Proteins 2004 57: 702-710     *'
      write(*,*)'* For comments, please email to: yzhang@ku.edu       ',
     &     '                       *'
      write(*,*)'*****************************************************',
     &     '************************'
      write(*,*)
      write(*,501)pdb(1),nseqA
 501  format('Structure1: ',A10,'  Length= ',I4)
      write(*,502)pdb(2),nseqB
 502  format('Structure2: ',A10,'  Length= ',I4,
     &     ' (by which all scores are normalized)')
      write(*,503)n_ali
 503  format('Number of residues in common= ',I4)
      write(*,513)rmsd_ali
 513  format('RMSD of  the common residues= ',F8.3)
      write(*,*)
      write(*,504)score_max,d0,score10_max
 504  format('TM-score    = ',f6.4,'  (d0=',f5.2,',',' TM10= ',f6.4,')')
      write(*,505)score_maxsub_max
 505  format('MaxSub-score= ',f6.4,'  (d0= 3.50)')
      score_GDT=(n_GDT1_max+n_GDT2_max+n_GDT4_max+n_GDT8_max)
     &     /float(4*nseqB)
      write(*,506)score_GDT,n_GDT1_max/float(nseqB),n_GDT2_max
     &     /float(nseqB),n_GDT4_max/float(nseqB),n_GDT8_max/float(nseqB)
 506  format('GDT-TS-score= ',f6.4,' %(d<1)=',f6.4,' %(d<2)=',f6.4,
     $     ' %(d<4)=',f6.4,' %(d<8)=',f6.4)
      score_GDT_HA=(n_GDT05_max+n_GDT1_max+n_GDT2_max+n_GDT4_max)
     &     /float(4*nseqB)
      write(*,507)score_GDT_HA,n_GDT05_max/float(nseqB),n_GDT1_max
     &     /float(nseqB),n_GDT2_max/float(nseqB),n_GDT4_max/float(nseqB)
 507  format('GDT-HA-score= ',f6.4,' %(d<0.5)=',f6.4,' %(d<1)=',f6.4,
     $     ' %(d<2)=',f6.4,' %(d<4)=',f6.4)
      write(*,*)
      
***   recall and output the superposition of maxiumum TM-score:
      LL=0
      do i=1,ka0
         m=k_ali0(i)            !record of the best alignment
         r_1(1,i)=xa(iA(m))
         r_1(2,i)=ya(iA(m))
         r_1(3,i)=za(iA(m))
         r_2(1,i)=xb(iB(m))
         r_2(2,i)=yb(iB(m))
         r_2(3,i)=zb(iB(m))
         LL=LL+1
      enddo
      call u3b(w,r_1,r_2,LL,1,rms,u,t,ier) !u rotate r_1 to r_2
      do j=1,nseqA
         xt(j)=t(1)+u(1,1)*xa(j)+u(1,2)*ya(j)+u(1,3)*za(j)
         yt(j)=t(2)+u(2,1)*xa(j)+u(2,2)*ya(j)+u(2,3)*za(j)
         zt(j)=t(3)+u(3,1)*xa(j)+u(3,2)*ya(j)+u(3,3)*za(j)
      enddo

********* extract rotation matrix ------------>
      write(*,*)'-------- rotation matrix to rotate Chain-1 to ',
     &     'Chain-2 ------'
      write(*,*)'i          t(i)         u(i,1)         u(i,2) ',
     &     '        u(i,3)'
      do i=1,3
         write(*,304)i,t(i),u(i,1),u(i,2),u(i,3)
      enddo
c      do j=1,nseqA
c         xt(j)=t(1)+u(1,1)*xa(j)+u(1,2)*ya(j)+u(1,3)*za(j)
c         yt(j)=t(2)+u(2,1)*xa(j)+u(2,2)*ya(j)+u(2,3)*za(j)
c         zt(j)=t(3)+u(3,1)*xa(j)+u(3,2)*ya(j)+u(3,3)*za(j)
c         write(*,*)j,xt(j),yt(j),zt(j)
c      enddo
      write(*,*)
 304  format(I2,f18.10,f15.10,f15.10,f15.10)

********* rmsd in superposed regions --------------->
      d=d_output                !for output
      call score_fun()          !give i_ali(i), score_max=score now
      LL=0
      do i=1,n_cut
         m=i_ali(i)             ![1,nseqA]
         r_1(1,i)=xa(iA(m))
         r_1(2,i)=ya(iA(m))
         r_1(3,i)=za(iA(m))
         r_2(1,i)=xb(iB(m))
         r_2(2,i)=yb(iB(m))
         r_2(3,i)=zb(iB(m))
         LL=LL+1
      enddo
      call u3b(w,r_1,r_2,LL,0,rms,u,t,ier)
      armsd=dsqrt(rms/LL)
      rmsd=armsd
      
***   output rotated chain1 + chain2----->
      if(m_out.ne.1)goto 999
      OPEN(unit=7,file=outname,status='unknown') !pdb1.aln + pdb2.aln
 900  format(A)
 901  format('select ',I4)
      write(7,900)'load inline'
      write(7,900)'select atomno<1000'
c      write(7,900)'color [255,20,147]'
      write(7,900)'wireframe .45'
      write(7,900)'select none'
      write(7,900)'select atomno>1000'
c      write(7,900)'color [100,149,237]'
      write(7,900)'wireframe .15'
      write(7,900)'color white'
      do i=1,n_cut
         write(7,901)nresA(iA(i_ali(i)))
         write(7,900)'color red'
      enddo
      write(7,900)'select all'
      write(7,900)'exit'
      write(7,514)rmsd_ali
 514  format('REMARK  RMSD of the common residues=',F8.3)
      write(7,515)score_max,d0
 515  format('REMARK  TM-score=',f6.4,' (d0=',f5.2,')')
      do i=1,nseqA
         write(7,1237)nresA(i),seqA(i),nresA(i),xt(i),yt(i),zt(i)
      enddo
      write(7,1238)
      do i=2,nseqA
         write(7,1239)nresA(i-1),nresA(i)
      enddo
      do i=1,nseqB
         write(7,1237)2000+nresB(i),seqB(i),nresB(i),xb(i),yb(i),zb(i)
      enddo
      write(7,1238)
      do i=2,nseqB
         write(7,1239)2000+nresB(i-1),2000+nresB(i)
      enddo
 1237 format('ATOM  ',i5,'  CA  ',A3,I6,4X,3F8.3)
 1238 format('TER')
 1239 format('CONECT',I5,I5)
 999  continue

***   record aligned residues by i=[1,nseqA], for sequenceM()------------>
      do i=1,nseqA
         iq(i)=0
      enddo
      do i=1,n_cut
         j=iA(i_ali(i))         ![1,nseqA]
         k=iB(i_ali(i))         ![1,nseqB]
         dis=sqrt((xt(j)-xb(k))**2+(yt(j)-yb(k))**2+(zt(j)-zb(k))**2)
         if(dis.lt.d_output)then
            iq(j)=1
         endif
      enddo
*******************************************************************
***   output aligned sequences
      k=0
      i=1
      j=1
 800  continue
      if(i.gt.nseqA.and.j.gt.nseqB)goto 802
      if(i.gt.nseqA.and.j.le.nseqB)then
         k=k+1
         sequenceA(k)='-'
         sequenceB(k)=seq1B(j)
         sequenceM(k)=' '
         j=j+1
         goto 800
      endif
      if(i.le.nseqA.and.j.gt.nseqB)then
         k=k+1
         sequenceA(k)=seq1A(i)
         sequenceB(k)='-'
         sequenceM(k)=' '
         i=i+1
         goto 800
      endif
      if(nresA(i).eq.nresB(j))then
         k=k+1
         sequenceA(k)=seq1A(i)
         sequenceB(k)=seq1B(j)
         if(iq(i).eq.1)then
            sequenceM(k)=':'
         else
            sequenceM(k)=' '
         endif
         i=i+1
         j=j+1
         goto 800
      elseif(nresA(i).lt.nresB(j))then
         k=k+1
         sequenceA(k)=seq1A(i)
         sequenceB(k)='-'
         sequenceM(k)=' '
         i=i+1
         goto 800
      elseif(nresB(j).lt.nresA(i))then
         k=k+1
         sequenceA(k)='-'
         sequenceB(k)=seq1B(j)
         sequenceM(k)=' '
         j=j+1
         goto 800
      endif
 802  continue

      write(*,600)d_output,n_cut,rmsd
 600  format('Superposition in the TM-score: Length(d<',f3.1,
     $     ')=',i3,'  RMSD=',f6.2)
      write(*,603)d_output
 603  format('(":" denotes the residue pairs of distance < ',f3.1,
     &     ' Angstrom)')
      write(*,601)(sequenceA(i),i=1,k)
      write(*,601)(sequenceM(i),i=1,k)
      write(*,601)(sequenceB(i),i=1,k)
      write(*,602)(mod(i,10),i=1,k)
 601  format(2000A1)
 602  format(2000I1)
      write(*,*)

c^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
 9999 END

ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c     1, collect those residues with dis<d;
c     2, calculate score_GDT, score_maxsub, score_TM
ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      subroutine score_fun
      PARAMETER(nmax=3000)

      common/stru/xt(nmax),yt(nmax),zt(nmax),xb(nmax),yb(nmax),zb(nmax)
      common/nres/nresA(nmax),nresB(nmax),nseqA,nseqB
      common/para/d,d0,d0_fix
      common/align/n_ali,iA(nmax),iB(nmax)
      common/nscore/i_ali(nmax),n_cut ![1,n_ali],align residues for the score
      common/scores/score,score_maxsub,score_fix,score10
      common/GDT/n_GDT05,n_GDT1,n_GDT2,n_GDT4,n_GDT8
      double precision score,score_max,score_fix,score_fix_max
      double precision score_maxsub,score10

      d_tmp=d
 21   n_cut=0                   !number of residue-pairs dis<d, for iteration
      n_GDT05=0                 !for GDT-score, # of dis<0.5
      n_GDT1=0                  !for GDT-score, # of dis<1
      n_GDT2=0                  !for GDT-score, # of dis<2
      n_GDT4=0                  !for GDT-score, # of dis<4
      n_GDT8=0                  !for GDT-score, # of dis<8
      score_maxsub_sum=0        !Maxsub-score
      score_sum=0               !TMscore
      score_sum10=0             !TMscore10
      do k=1,n_ali
         i=iA(k)                ![1,nseqA] reoder number of structureA
         j=iB(k)                ![1,nseqB]
         dis=sqrt((xt(i)-xb(j))**2+(yt(i)-yb(j))**2+(zt(i)-zb(j))**2)
***   for iteration:
         if(dis.lt.d_tmp)then
            n_cut=n_cut+1
            i_ali(n_cut)=k      ![1,n_ali], mark the residue-pairs in dis<d
         endif
***   for GDT-score:
         if(dis.le.8)then
            n_GDT8=n_GDT8+1
            if(dis.le.4)then
               n_GDT4=n_GDT4+1
               if(dis.le.2)then
                  n_GDT2=n_GDT2+1
                  if(dis.le.1)then
                     n_GDT1=n_GDT1+1
                     if(dis.le.0.5)then
                        n_GDT05=n_GDT05+1
                     endif
                  endif
               endif
            endif
         endif
***   for MAXsub-score:
         if(dis.lt.3.5)then
            score_maxsub_sum=score_maxsub_sum+1/(1+(dis/3.5)**2)
         endif
***   for TM-score:
         score_sum=score_sum+1/(1+(dis/d0)**2)
***   for TM-score10:
         if(dis.lt.10)then
            score_sum10=score_sum10+1/(1+(dis/d0)**2)
         endif
      enddo
      if(n_cut.lt.3.and.n_ali.gt.3)then
         d_tmp=d_tmp+.5
         goto 21
      endif
      score_maxsub=score_maxsub_sum/float(nseqB) !MAXsub-score
      score=score_sum/float(nseqB) !TM-score
      score10=score_sum10/float(nseqB) !TM-score10
      
      return
      end

cccccccccccccccc Calculate sum of (r_d-r_m)^2 cccccccccccccccccccccccccc
c  w    - w(m) is weight for atom pair  c m           (given)
c  x    - x(i,m) are coordinates of atom c m in set x       (given)
c  y    - y(i,m) are coordinates of atom c m in set y       (given)
c  n    - n is number of atom pairs                         (given)
c  mode  - 0:calculate rms only                             (given)
c          1:calculate rms,u,t                              (takes longer)
c  rms   - sum of w*(ux+t-y)**2 over all atom pairs         (result)
c  u    - u(i,j) is   rotation  matrix for best superposition  (result)
c  t    - t(i)   is translation vector for best superposition  (result)
c  ier  - 0: a unique optimal superposition has been determined(result)
c       -1: superposition is not unique but optimal
c       -2: no result obtained because of negative weights w
c           or all weights equal to zero.
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      subroutine u3b(w, x, y, n, mode, rms, u, t, ier)
      integer ip(9), ip2312(4), i, j, k, l, m1, m, ier, n, mode
      double precision w(1), x(3, 1), y(3, 1), u(3, 3), t(3), rms, sigma
      double precision r(3, 3), xc(3), yc(3), wc, a(3, 3), b(3, 3), e0, 
     &e(3), e1, e2, e3, d, spur, det, cof, h, g, cth, sth, sqrth, p, tol
     &, rr(6), rr1, rr2, rr3, rr4, rr5, rr6, ss(6), ss1, ss2, ss3, ss4, 
     &ss5, ss6, zero, one, two, three, sqrt3
      equivalence (rr(1), rr1), (rr(2), rr2), (rr(3), rr3), (rr(4), rr4)
     &, (rr(5), rr5), (rr(6), rr6), (ss(1), ss1), (ss(2), ss2), (ss(3), 
     &ss3), (ss(4), ss4), (ss(5), ss5), (ss(6), ss6), (e(1), e1), (e(2)
     &, e2), (e(3), e3)
      data sqrt3 / 1.73205080756888d+00 /
      data tol / 1.0d-2 /
      data zero / 0.0d+00 /
      data one / 1.0d+00 /
      data two / 2.0d+00 /
      data three / 3.0d+00 /
      data ip / 1, 2, 4, 2, 3, 5, 4, 5, 6 /
      data ip2312 / 2, 3, 1, 2 /
c 156 "rms.for"
      wc = zero
      rms = 0.0
      e0 = zero
      do 1 i = 1, 3
      xc(i) = zero
      yc(i) = zero
      t(i) = 0.0
      do 1 j = 1, 3
      d = zero
      if (i .eq. j) d = one
      u(i,j) = d
      a(i,j) = d
    1 r(i,j) = zero
      ier = -1
c**** DETERMINE CENTROIDS OF BOTH VECTOR SETS X AND Y
c 170 "rms.for"
      if (n .lt. 1) return 
c 172 "rms.for"
      ier = -2
      do 2 m = 1, n
      if (w(m) .lt. 0.0) return 
      wc = wc + w(m)
      do 2 i = 1, 3
      xc(i) = xc(i) + (w(m) * x(i,m))
    2 yc(i) = yc(i) + (w(m) * y(i,m))
      if (wc .le. zero) return 
      do 3 i = 1, 3
      xc(i) = xc(i) / wc
c**** DETERMINE CORRELATION MATRIX R BETWEEN VECTOR SETS Y AND X
c 182 "rms.for"
    3 yc(i) = yc(i) / wc
c 184 "rms.for"
      do 4 m = 1, n
      do 4 i = 1, 3
      e0 = e0 + (w(m) * (((x(i,m) - xc(i)) ** 2) + ((y(i,m) - yc(i)) ** 
     &2)))
c 187 "rms.for"
      d = w(m) * (y(i,m) - yc(i))
      do 4 j = 1, 3
c**** CALCULATE DETERMINANT OF R(I,J)
c 189 "rms.for"
    4 r(i,j) = r(i,j) + (d * (x(j,m) - xc(j)))
c 191 "rms.for"
      det = ((r(1,1) * ((r(2,2) * r(3,3)) - (r(2,3) * r(3,2)))) - (r(1,2
     &) * ((r(2,1) * r(3,3)) - (r(2,3) * r(3,1))))) + (r(1,3) * ((r(2,1)
     & * r(3,2)) - (r(2,2) * r(3,1))))
c**** FORM UPPER TRIANGLE OF TRANSPOSED(R)*R
c 194 "rms.for"
      sigma = det
c 196 "rms.for"
      m = 0
      do 5 j = 1, 3
      do 5 i = 1, j
      m = m + 1
c***************** EIGENVALUES *****************************************
c**** FORM CHARACTERISTIC CUBIC  X**3-3*SPUR*X**2+3*COF*X-DET=0
c 200 "rms.for"
    5 rr(m) = ((r(1,i) * r(1,j)) + (r(2,i) * r(2,j))) + (r(3,i) * r(3,j)
     &)
c 203 "rms.for"
      spur = ((rr1 + rr3) + rr6) / three
      cof = ((((((rr3 * rr6) - (rr5 * rr5)) + (rr1 * rr6)) - (rr4 * rr4)
     &) + (rr1 * rr3)) - (rr2 * rr2)) / three
c 205 "rms.for"
      det = det * det
      do 6 i = 1, 3
    6 e(i) = spur
c**** REDUCE CUBIC TO STANDARD FORM Y**3-3HY+2G=0 BY PUTTING X=Y+SPUR
c 208 "rms.for"
      if (spur .le. zero) goto 40
c 210 "rms.for"
      d = spur * spur
      h = d - cof
c**** SOLVE CUBIC. ROOTS ARE E1,E2,E3 IN DECREASING ORDER
c 212 "rms.for"
      g = (((spur * cof) - det) / two) - (spur * h)
c 214 "rms.for"
      if (h .le. zero) goto 8
      sqrth = dsqrt(h)
      d = ((h * h) * h) - (g * g)
      if (d .lt. zero) d = zero
      d = datan2(dsqrt(d),- g) / three
      cth = sqrth * dcos(d)
      sth = (sqrth * sqrt3) * dsin(d)
      e1 = (spur + cth) + cth
      e2 = (spur - cth) + sth
      e3 = (spur - cth) - sth
c.....HANDLE SPECIAL CASE OF 3 IDENTICAL ROOTS
c 224 "rms.for"
      if (mode) 10, 50, 10
c**************** EIGENVECTORS *****************************************
c 226 "rms.for"
    8 if (mode) 30, 50, 30
c 228 "rms.for"
   10 do 15 l = 1, 3, 2
      d = e(l)
      ss1 = ((d - rr3) * (d - rr6)) - (rr5 * rr5)
      ss2 = ((d - rr6) * rr2) + (rr4 * rr5)
      ss3 = ((d - rr1) * (d - rr6)) - (rr4 * rr4)
      ss4 = ((d - rr3) * rr4) + (rr2 * rr5)
      ss5 = ((d - rr1) * rr5) + (rr2 * rr4)
      ss6 = ((d - rr1) * (d - rr3)) - (rr2 * rr2)
      j = 1
      if (dabs(ss1) .ge. dabs(ss3)) goto 12
      j = 2
      if (dabs(ss3) .ge. dabs(ss6)) goto 13
   11 j = 3
      goto 13
   12 if (dabs(ss1) .lt. dabs(ss6)) goto 11
   13 d = zero
      j = 3 * (j - 1)
      do 14 i = 1, 3
      k = ip(i + j)
      a(i,l) = ss(k)
   14 d = d + (ss(k) * ss(k))
      if (d .gt. zero) d = one / dsqrt(d)
      do 15 i = 1, 3
   15 a(i,l) = a(i,l) * d
      d = ((a(1,1) * a(1,3)) + (a(2,1) * a(2,3))) + (a(3,1) * a(3,3))
      m1 = 3
      m = 1
      if ((e1 - e2) .gt. (e2 - e3)) goto 16
      m1 = 1
      m = 3
   16 p = zero
      do 17 i = 1, 3
      a(i,m1) = a(i,m1) - (d * a(i,m))
   17 p = p + (a(i,m1) ** 2)
      if (p .le. tol) goto 19
      p = one / dsqrt(p)
      do 18 i = 1, 3
   18 a(i,m1) = a(i,m1) * p
      goto 21
   19 p = one
      do 20 i = 1, 3
      if (p .lt. dabs(a(i,m))) goto 20
      p = dabs(a(i,m))
      j = i
   20 continue
      k = ip2312(j)
      l = ip2312(j + 1)
      p = dsqrt((a(k,m) ** 2) + (a(l,m) ** 2))
      if (p .le. tol) goto 40
      a(j,m1) = zero
      a(k,m1) = - (a(l,m) / p)
      a(l,m1) = a(k,m) / p
   21 a(1,2) = (a(2,3) * a(3,1)) - (a(2,1) * a(3,3))
      a(2,2) = (a(3,3) * a(1,1)) - (a(3,1) * a(1,3))
c****************** ROTATION MATRIX ************************************
c 282 "rms.for"
      a(3,2) = (a(1,3) * a(2,1)) - (a(1,1) * a(2,3))
c 284 "rms.for"
   30 do 32 l = 1, 2
      d = zero
      do 31 i = 1, 3
      b(i,l) = ((r(i,1) * a(1,l)) + (r(i,2) * a(2,l))) + (r(i,3) * a(3,l
     &))
c 288 "rms.for"
   31 d = d + (b(i,l) ** 2)
      if (d .gt. zero) d = one / dsqrt(d)
      do 32 i = 1, 3
   32 b(i,l) = b(i,l) * d
      d = ((b(1,1) * b(1,2)) + (b(2,1) * b(2,2))) + (b(3,1) * b(3,2))
      p = zero
      do 33 i = 1, 3
      b(i,2) = b(i,2) - (d * b(i,1))
   33 p = p + (b(i,2) ** 2)
      if (p .le. tol) goto 35
      p = one / dsqrt(p)
      do 34 i = 1, 3
   34 b(i,2) = b(i,2) * p
      goto 37
   35 p = one
      do 36 i = 1, 3
      if (p .lt. dabs(b(i,1))) goto 36
      p = dabs(b(i,1))
      j = i
   36 continue
      k = ip2312(j)
      l = ip2312(j + 1)
      p = dsqrt((b(k,1) ** 2) + (b(l,1) ** 2))
      if (p .le. tol) goto 40
      b(j,2) = zero
      b(k,2) = - (b(l,1) / p)
      b(l,2) = b(k,1) / p
   37 b(1,3) = (b(2,1) * b(3,2)) - (b(2,2) * b(3,1))
      b(2,3) = (b(3,1) * b(1,2)) - (b(3,2) * b(1,1))
      b(3,3) = (b(1,1) * b(2,2)) - (b(1,2) * b(2,1))
      do 39 i = 1, 3
      do 39 j = 1, 3
c****************** TRANSLATION VECTOR *********************************
c 320 "rms.for"
   39 u(i,j) = ((b(i,1) * a(j,1)) + (b(i,2) * a(j,2))) + (b(i,3) * a(j,3
     &))
   40 do 41 i = 1, 3
c********************** RMS ERROR **************************************
c 323 "rms.for"
   41 t(i) = ((yc(i) - (u(i,1) * xc(1))) - (u(i,2) * xc(2))) - (u(i,3)
     & * xc(3))
   50 do 51 i = 1, 3
      if (e(i) .lt. zero) e(i) = zero
   51 e(i) = dsqrt(e(i))
      ier = 0
      if (e2 .le. (e1 * 1.0d-05)) ier = -1
      d = e3
      if (sigma .ge. 0.0) goto 52
      d = - d
      if ((e2 - e3) .le. (e1 * 1.0d-05)) ier = -1
   52 d = (d + e2) + e1
      rms = (e0 - d) - d
      if (rms .lt. 0.0) rms = 0.0
      return 
c.....END U3B...........................................................
c----------------------------------------------------------
c                       THE END
c----------------------------------------------------------
c 338 "rms.for"
      end