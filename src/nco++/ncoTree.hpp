#ifndef INC_ncoTree_hpp_
#define INC_ncoTree_hpp_

#include <antlr/config.hpp>
#include "ncoParserTokenTypes.hpp"
/* $ANTLR 2.7.6 (20070220): "ncoGrammer.g" -> "ncoTree.hpp"$ */
#include <antlr/TreeParser.hpp>

#line 1 "ncoGrammer.g"

    #include <math.h>
    #include <malloc.h>
    #include <assert.h>
    #include <ctype.h>
    #include <iostream>
    #include <sstream>
    #include <string>
    #include <algorithm>
    #include "ncap2.hh"
    #include "NcapVar.hh"
    #include "NcapVarVector.hh"
    #include "sdo_utl.hh" // SDO stand-alone utilities: dbg/err/wrn_prn()
    #include "NcapVector.hh"
    ANTLR_USING_NAMESPACE(std);
    ANTLR_USING_NAMESPACE(antlr);
    

#line 29 "ncoTree.hpp"
class CUSTOM_API ncoTree : public ANTLR_USE_NAMESPACE(antlr)TreeParser, public ncoParserTokenTypes
{
#line 506 "ncoGrammer.g"


private:
    prs_sct *prs_arg;
    bool bcst;
    var_sct* var_cst;
public:
    void setTable(prs_sct *prs_in){
        prs_arg=prs_in;
    }
    // Customized Constructor
    ncoTree(prs_sct *prs_in){
        prs_arg=prs_in;
        // default is NO. Casting variable set to true 
        // causes casting in function out(). var_cst must 
        // then be defined 
        bcst=false;  
        ncoTree();
    }


int 
lmt_init(
RefAST aRef, 
vector<ast_lmt_sct> &ast_lmt_vtr) 
{
      const std::string fnc_nm("lmt_init"); // [sng] Function name   

      int idx;
      int nbr_dmn;   
      int nbr_cln; // Number of colons in limit
      RefAST lRef;
      RefAST eRef;
      RefAST cRef;
      ast_lmt_sct hyp;

      if(aRef->getType() != LMT_LIST)
         return 0;
    
      lRef=aRef->getFirstChild();

      nbr_dmn=lmt_peek(aRef);      
      //nbr_dmn=lRef->getNumberOfChildren();

      for(idx=0 ; idx < nbr_dmn ; idx++){
         hyp.ind[0]=ANTLR_USE_NAMESPACE(antlr)nullAST;
         hyp.ind[1]=ANTLR_USE_NAMESPACE(antlr)nullAST;
         hyp.ind[2]=ANTLR_USE_NAMESPACE(antlr)nullAST;

             
       if(lRef->getType()!=LMT) 
            return 0;
       
        eRef=lRef->getFirstChild();
        nbr_cln=0;
        
       while(eRef) {
          if(eRef->getType() == COLON){
            cRef=eRef;        
            nbr_cln++;
          }
           eRef=eRef->getNextSibling();
        }
      
      // Initialise  to default markers
       switch(nbr_cln) {
          case 0: 
             break;
                
         case 1: hyp.ind[0]=cRef;
                 hyp.ind[1]=cRef;
                 break;

         case 2: hyp.ind[0]=cRef;
                 hyp.ind[1]=cRef;
                 hyp.ind[2]=cRef;
                 break;

         default: err_prn(fnc_nm,"Too many hyperslab indices");
                  break;  
        }

       eRef=lRef->getFirstChild();
       // point inidices to any expressions that exist
        nbr_cln=0;
       while(eRef) {
          if(eRef->getType() == COLON) 
             nbr_cln++; 
           else   
             hyp.ind[nbr_cln]=eRef;
           
           eRef=eRef->getNextSibling();
       }
       // save indices 
       ast_lmt_vtr.push_back(hyp);

       lRef=lRef->getNextSibling();
     }
     return nbr_dmn;

} 

int 
lmt_mk(
RefAST lmt,
NcapVector<lmt_sct*> &lmt_vtr ) 
{   
int nbr_dmn;
int idx;
int jdx;
long lcl_ind[3];
char *buff;

var_sct *var_out;
lmt_sct *lmt_ptr;
RefAST aRef;

vector<ast_lmt_sct> ast_lmt_vtr;


// populate ast_lmt_vtr
nbr_dmn=lmt_init(lmt,ast_lmt_vtr);

  for(idx=0 ; idx <nbr_dmn ; idx++){


     lcl_ind[0]=-2; lcl_ind[1]=-2; lcl_ind[2]=0; 

    for(jdx=0 ; jdx <3 ; jdx++){

     aRef=ast_lmt_vtr[idx].ind[jdx];

     if(!aRef)
        continue; //do nothing - use default lcl_ind values     
     else if( aRef->getType() == COLON){
       if(jdx <2) lcl_ind[jdx]=-1;
     }else{
         // Calculate number using out()
         var_out=out(aRef);

         // convert result to type int
          var_out=nco_var_cnf_typ(NC_INT,var_out);    
         (void)cast_void_nctype(NC_INT,&var_out->val);

          // only interested in the first value.
         lcl_ind[jdx]=var_out->val.lp[0];

         var_out=nco_var_free(var_out);
        }
     }// end jdx
         
     // fill out lmt structure
     // use same logic as nco_lmt_prs 
     lmt_ptr=(lmt_sct*)nco_calloc((size_t)1,sizeof(lmt_sct));

     lmt_ptr->nm=NULL;
     //lmt_ptr->lmt_typ=-1;
     lmt_ptr->is_usr_spc_lmt=True; /* True if any part of limit is user-specified, else False */
     lmt_ptr->min_sng=NULL;
     lmt_ptr->max_sng=NULL;
     lmt_ptr->srd_sng=NULL;
     /* rec_skp_nsh_spf is used for record dimension in multi-file operators */
     lmt_ptr->rec_skp_nsh_spf=0L; /* Number of records skipped in initial superfluous files */
    
    
    /* Fill in structure */
    if( lcl_ind[0] >= 0) {
          buff=(char*)nco_malloc(20*sizeof(char));
          (void)sprintf(buff, "%ld",lcl_ind[0]);
           lmt_ptr->min_sng=buff; 
    }    

    /* Fill in structure */
    if( lcl_ind[1] >= 0) {
          buff=(char*)nco_malloc(20*sizeof(char));
          (void)sprintf(buff, "%ld",lcl_ind[1]);
           lmt_ptr->max_sng=buff;
    }    

    /* Fill in structure */
    if( lcl_ind[2] > 0) {
          buff=(char*)nco_malloc(20*sizeof(char));
          (void)sprintf(buff, "%ld",lcl_ind[2]);
           lmt_ptr->srd_sng=buff;
    }    

    /* need to deal with situation where only start is defined -- ie picking only a single value */
    if(lcl_ind[0] >=0 && lcl_ind[1]==-2){
          buff=(char*)nco_malloc(20*sizeof(char));
          (void)sprintf(buff, "%ld",lcl_ind[0]);
          lmt_ptr->max_sng=buff; 
    }    


    if(lmt_ptr->max_sng == NULL) lmt_ptr->is_usr_spc_max=False; else lmt_ptr->is_usr_spc_max=True;
    if(lmt_ptr->min_sng == NULL) lmt_ptr->is_usr_spc_min=False; else lmt_ptr->is_usr_spc_min=True;

    lmt_vtr.push_back(lmt_ptr);


   } // end idx

   return nbr_dmn;

} /* end lmt_mk */



public:
    void run(RefAST tr){
        while(tr) {
          (void)statements(tr);   
          tr=tr->getNextSibling();   
        }
    }

public:
    void run_dbl(RefAST tr,int icnt){
     int idx=0;
     RefAST ntr=tr;
      
     //small list dont bother with double parsing     
     if(icnt <4) goto small;

     //Initial scan
     prs_arg->ntl_scn=True;
     while(idx++ < icnt){
       (void)statements(ntr);   
       ntr=ntr->getNextSibling();   
     }

     //Define variables in output
    (void)nco_redef(prs_arg->out_id);  
    (void)ncap_def_ntl_scn(prs_arg);
    (void)nco_enddef(prs_arg->out_id);  


    // Sort expressions - MPI preparation
    if(prs_arg->NCAP_MPI_SORT){  
       prs_arg->ntl_scn=False;
      // nb A vector of vectors
      std::vector< std::vector<RefAST> > all_ast_vtr;

      // Populate  vector 
      (void)ncap_mpi_srt(tr,icnt,all_ast_vtr);

      // Evaluate expressions
      for(unsigned vtr_idx=0 ; vtr_idx<all_ast_vtr.size(); vtr_idx++)
        for(unsigned jdx=0 ; jdx<all_ast_vtr[vtr_idx].size(); jdx++)
	     (void)statements(all_ast_vtr[vtr_idx][jdx]);
        
    goto end;
    } //end if

   


small: 
     idx=0;
     ntr=tr;
     //Final scan
     prs_arg->ntl_scn=False;
     while(idx++ < icnt){
       (void)statements(ntr);   
       ntr=ntr->getNextSibling();   
     }
end: ;
   }

public:
    void run_exe(RefAST tr){
    // number of statements in block
    int nbr_stmt=0;
    int idx;
    int icnt=0;
    int gtyp;
    
    RefAST etr=ANTLR_USE_NAMESPACE(antlr)nullAST;
    RefAST ntr;
   
    ntr=tr;
    do nbr_stmt++; 
    while(ntr=ntr->getNextSibling());        
            
     
    
    if(nbr_stmt <4){
        prs_arg->ntl_scn=False;
        ntr=tr;
        do (void)statements(ntr);   
        while(ntr=ntr->getNextSibling());   
        goto exit;
    }
  
    ntr=tr;

    for(idx=0 ; idx < nbr_stmt; idx++){
      gtyp=ntr->getType();
      // we have hit an IF or a code block
      if(gtyp==BLOCK || gtyp==IF ||gtyp==DEFDIM ) {
        if(icnt>0) 
         (void)run_dbl(etr,icnt);
        icnt=0;
        etr=ANTLR_USE_NAMESPACE(antlr)nullAST;; 
        prs_arg->ntl_scn=False;
        (void)statements(ntr);      
       }

       if(gtyp==EXPR || gtyp== NULL_NODE) 
        if(icnt++==0) etr=ntr;
        
       
      ntr=ntr->getNextSibling();
      
    } // end for
    if(icnt >0)
       (void)run_dbl(etr,icnt);      

      
exit: ;     
            

    } // end run_exe


#line 33 "ncoTree.hpp"
public:
	ncoTree();
	static void initializeASTFactory( ANTLR_USE_NAMESPACE(antlr)ASTFactory& factory );
	int getNumTokens() const
	{
		return ncoTree::NUM_TOKENS;
	}
	const char* getTokenName( int type ) const
	{
		if( type > getNumTokens() ) return 0;
		return ncoTree::tokenNames[type];
	}
	const char* const* getTokenNames() const
	{
		return ncoTree::tokenNames;
	}
	public: int  lmt_peek(ANTLR_USE_NAMESPACE(antlr)RefAST _t);
	public: void statements(ANTLR_USE_NAMESPACE(antlr)RefAST _t);
	public: var_sct * assign_ntl(ANTLR_USE_NAMESPACE(antlr)RefAST _t,
		bool bram
	);
	public: var_sct * assign(ANTLR_USE_NAMESPACE(antlr)RefAST _t,
		bool bram
	);
	public: var_sct * out(ANTLR_USE_NAMESPACE(antlr)RefAST _t);
	public: var_sct * out_asn(ANTLR_USE_NAMESPACE(antlr)RefAST _t);
public:
	ANTLR_USE_NAMESPACE(antlr)RefAST getAST()
	{
		return returnAST;
	}
	
protected:
	ANTLR_USE_NAMESPACE(antlr)RefAST returnAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST _retTree;
private:
	static const char* tokenNames[];
#ifndef NO_STATIC_CONSTS
	static const int NUM_TOKENS = 103;
#else
	enum {
		NUM_TOKENS = 103
	};
#endif
	
};

#endif /*INC_ncoTree_hpp_*/
