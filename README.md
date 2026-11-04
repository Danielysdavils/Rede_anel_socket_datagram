Projeto de rede em anel para disciplina de Redes 1.

São 4 máquinas se comunicando via Sockets Datagram (UDP), usando a lógica de passagem de bastão (token ring). Não há servidor fixo e uso de broadcast.

**Funcionamento do Jogo**

A cada rodada, a máquina com o bastão vira a roleta e os outros 3 viram jogadores. Todos começam com 12.000 fichas. O jogo é dividido em 4 eventos: 

- Início: 
    A roleta anuncia o início do jogo enviando a mensagem inicial.

- Fase de Apostas:
    Cada jogador deve apostar um valor entre 10 e 100 fichas e escolher o modo de jogo:
    -- Número exato x (0 a 36): Se acertar, ganha x vezes o valor apostado.
    -- Cor (Preto ou Vermelho): Se acertar, ganha o quádruplo do valor apostado.
    -- Par ou Ímpar: Se acertar, ganha o dobro do valor apostado. 

    Os jogadores enviam as suas apostas pelo anel até chegar na roleta. A roleta recebe as apostas, recolhe as fichas e guarda no seu cofre.

- Sorteio: 
    Ocorre-m os sorteios, as apostas são verificadas e obtêm-se os resultados.

- Pagamento: 
    -- Se o jogador perdeu, a roleta fica com as fichas dele.
    -- Se o jogador ganhou, a roleta envia o prêmio pelo anel até o vencedor.

    Após todos os prêmios serem pagos e os saldos atualizados, a rodada acaba. E o bastão indicador da roleta passa para a próxima máquina e ela se torna automaticamente a nova roleta.

O jogo termina após todas as máquinas serem uma vez a roleta; após isso, é mostrado o placar final.